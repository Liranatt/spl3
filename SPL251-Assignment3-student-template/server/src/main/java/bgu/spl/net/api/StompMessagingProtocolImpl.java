package bgu.spl.net.api;

import bgu.spl.net.impl.stomp.Frames.ErrorFrame;
import bgu.spl.net.impl.stomp.Frames.Frame;
import bgu.spl.net.impl.stomp.Frames.MessageFrame;
import bgu.spl.net.impl.stomp.Frames.ReceiptFrame;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.ConnectionsImpl;

import java.util.HashMap;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<Frame> {

    private int connectionId;
    private ConnectionsImpl<Frame> connections;
    private boolean shouldTerminate;
    private HashMap<String, Integer> topicsIds;


    @Override
    public void start(int connectionId, Connections<Frame> connections) {
        this.connections = (ConnectionsImpl<Frame>) connections;
        this.connectionId = connectionId;
        this.topicsIds = new HashMap<>();
        this.shouldTerminate = false;
    }

    @Override
    public void process(Frame message) {
        System.out.println("arrived here:\n" + message.toString());
        switch (message.getCommand()) {
            case "CONNECT":
                if (!"1.2".equals(message.getHeaders().get("accept - version"))) {
                    sendError("accept - version is not valid", message);
                    break;
                }
                if (!"stomp.cs.bgu.ac.il".equals(message.getHeaders().get("host"))) {
                    sendError("host name is not valid", message);
                    break;
                }
                String login = message.getHeaders().get("login");
                String passcode = message.getHeaders().get("passcode");
                if (login == null || passcode == null || !connections.addLogin(login, passcode)) {
                    sendError("login and passcode error, didn't found them or the login already exists", message);
                    break;
                }
                break;
            case "SEND":
                String sendTopic = message.getHeaders().get("destination");
                if (sendTopic != null) {
                    if (connections.isSubscribed(connectionId, sendTopic)) {
                        MessageFrame newMsg = new MessageFrame();
                        newMsg.addHeader("destination", sendTopic);
                        newMsg.addBody(message.getBody());
                        connections.send(sendTopic, message); /// TODO: add headers: subscription + message - id;
                    }
                    else {
                        sendError("you can't send a message to a topic you are not subscribed to", message);
                    }
                }
                else {
                    sendError("did not contain destination header which is REQUIRED for SEND message", message);
                }
                break;
            case "SUBSCRIBE":
                String subsTopic = message.getHeaders().get("destination");
                if (subsTopic != null) {
                    connections.subscribeToChannel(connectionId, subsTopic);
                    if (message.getHeaders().get("id") != null) {
                        try {
                            topicsIds.put(subsTopic, Integer.parseInt(message.getHeaders().get("id")));
                        } catch (NumberFormatException e) {
                            System.out.println("server received SUBSCRIBE frame with illegal number format in id header");
                        }
                    }
                }
                else {
                    sendError("did not contain destination header which is REQUIRED for SUBSCRIBE message", message);
                }
                break;
            case "UNSUBSCRIBE":
                try {
                    int unSubsId = Integer.parseInt(message.getHeaders().get("id"));
                    String topic = getTopicFromId(unSubsId);
                    if (topic != null) {
                        connections.unSubscribeFromChannel(connectionId, topic);
                    }
                    else {
                        sendError("wrong id header in UNSUBSCRIBE message", message);
                    }
                } catch (NumberFormatException e) {
                    System.out.println("server received UNSUBSCRIBE frame with illegal number format in id header");
                    sendError("did not contain id header which is REQUIRED for UNSUBSCRIBE message", message);
                }
                break;
            case "DISCONNECT":
                connections.disconnect(connectionId);
                try {
                    int receiptId = Integer.parseInt(message.getHeaders().get("receipt"));
                    ReceiptFrame receipt = new ReceiptFrame();
                    receipt.addHeader("receipt - id", receiptId + "");
                    connections.send(connectionId, receipt);
                } catch (NumberFormatException e) {
                    System.out.println("server received SUBSCRIBE frame with illegal number format");
                    sendError("did not contain id header which is REQUIRED for DISCONNECT message", message);
                }
                break;

        }
    }

    private void sendError(String errorMessage, Frame message) {
        ErrorFrame error = new ErrorFrame();
        error.addHeader("message", "malformed frame received");
        error.addHeader("receipt - id", message.getHeaders().get("receipt")); // may not work if there isn't a receipt but still exception safe
        String body = "The message:\n-----" + message.getBody() + "-----\n" + errorMessage;
        error.addBody(body);
        System.out.println(error);
        connections.send(connectionId, error);
        shouldTerminate = true;
    }


    private String getTopicFromId(int id) {
        for (String key : topicsIds.keySet())
            if (topicsIds.get(key) == id)
                return key;
        return null;
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
}
