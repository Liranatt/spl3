package bgu.spl.net.api;

import bgu.spl.net.impl.stomp.*;
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
        if (message instanceof ConnectFrame) {
            connections.connect(connectionId);
        }
//        else if (message instanceof SendFrame) {}
//        else if (message instanceof SubscribeFrame) {}
//        else if (message instanceof UnSubscribeFrame) {}
//        else if (message instanceof DisconnectFrame) {}
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }
}
