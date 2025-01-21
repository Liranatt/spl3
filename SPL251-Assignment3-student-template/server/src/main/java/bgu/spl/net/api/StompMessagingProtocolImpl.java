package bgu.spl.net.api;

import bgu.spl.net.impl.stomp.Frames.Frame;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.ConnectionsImpl;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<Frame> {

    int connectionId;
    ConnectionsImpl<Frame> connections;
    boolean shouldTerminate;


    @Override
    public void start(int connectionId, Connections<Frame> connections) {
        this.connections = (ConnectionsImpl<Frame>) connections;
        this.connectionId = connectionId;
        this.shouldTerminate = false;
    }

    @Override
    public void process(Frame message) {
        switch (message.getCommand()) {
            case "CONNECT":
                break;
            case "SEND":
                break;
            case "SUBSCRIBE":
                break;
            case "UNSUBSCRIBE":
                break;
            case "DISCONNECT":
                break;

        }
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
}
