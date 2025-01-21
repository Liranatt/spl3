package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class ConnectionsImpl<T> implements Connections<T> {
    ConcurrentHashMap<Integer, ConnectionHandler<T>> connectionHandlers;
    ConcurrentHashMap<String, ConcurrentLinkedQueue<Integer>> channelsSubscription;

    public ConnectionsImpl() {
        connectionHandlers = new ConcurrentHashMap<>();
        channelsSubscription = new ConcurrentHashMap<>();
    }

    public void subscribeToChannel(int connectionId, String channel) {}
    public void unSubscribeFromChannel(int connectionId, String channel) {}

    public void connect(int connectionId) {
        if (connectionHandlers.containsKey(connectionId))
            System.out.println("ConnectionsImpl.ConnectFrame() has been called, already has a connectionId that equals = " + connectionId);
        else {
            System.out.println("ConnectionsImpl.ConnectFrame() has been called, connectionId = " + connectionId);
        }
    }

    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> con = connectionHandlers.get(connectionId);
        if (con != null) {
            con.send(msg);
            return true;
        }
        else
            return false;
    }

    @Override
    public void send(String channel, T msg) {
        if (channelsSubscription.containsKey(channel)) {
            for (Integer id : channelsSubscription.get(channel)) {
                send(id, msg);
            }
        }
    }

    @Override
    public void disconnect(int connectionId) {
        ConnectionHandler<T> con = connectionHandlers.remove(connectionId);
        if (con != null) {
            for (String key : channelsSubscription.keySet()) {
                channelsSubscription.get(key).remove(connectionId);
            }
        }
    }
}
