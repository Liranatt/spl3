package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class ConnectionsImpl<T> implements Connections<T> {

    private ConcurrentHashMap<Integer, ConnectionHandler<T>> connectionHandlers;
    private ConcurrentHashMap<String, ConcurrentLinkedQueue<Integer>> channelsSubscription;

    private static class singletonHolder {
        private static final ConnectionsImpl<?> instance = new ConnectionsImpl<>();
    }

    @SuppressWarnings("unchecked")
    public static <T> ConnectionsImpl<T> getInstance() { return (ConnectionsImpl<T>) singletonHolder.instance;}

    private ConnectionsImpl() {
        connectionHandlers = new ConcurrentHashMap<>();
        channelsSubscription = new ConcurrentHashMap<>();
    }

    public void subscribeToChannel(int connectionId, String channel) {}

    public void unSubscribeFromChannel(int connectionId, String channel) {}

    public void addConnection(ConnectionHandler<T> connection, int connectionId) {
        connectionHandlers.put(connectionId, connection);
    }

    ///  saves the connection handler that called this method and returns its connection id
//    public int connect(ConnectionHandler<T> connection) {
//        connectionHandlers.put(connectionsCounter++, connection);
//        return (connectionsCounter - 1);
//    }
//    public void connect(int connectionId) {
//        if (connectionHandlers.containsKey(connectionId))
//            System.out.println("ConnectionsImpl.ConnectFrame() has been called, already has a connectionId that equals = " + connectionId);
//        else {
//            System.out.println("ConnectionsImpl.ConnectFrame() has been called, connectionId = " + connectionId);
//        }
//    }

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
