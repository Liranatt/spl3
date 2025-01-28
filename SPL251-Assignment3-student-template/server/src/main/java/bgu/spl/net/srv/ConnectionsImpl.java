package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class ConnectionsImpl<T> implements Connections<T> {

    private final ConcurrentHashMap<Integer, ConnectionHandler<T>> connectionHandlers;
    private final ConcurrentHashMap<String, ConcurrentLinkedQueue<Integer>> channelsSubscription;
    private final ConcurrentHashMap<String, String> loginInformation;
    private final ConcurrentHashMap<Integer, String> connectionIdToLogin;
    private final ConcurrentHashMap<String, Boolean> loginIsConnected;

    private static class singletonHolder {
        private static final ConnectionsImpl<?> instance = new ConnectionsImpl<>();
    }

    @SuppressWarnings("unchecked")
    public static <T> ConnectionsImpl<T> getInstance() { return (ConnectionsImpl<T>) singletonHolder.instance;}

    private ConnectionsImpl() {
        connectionHandlers = new ConcurrentHashMap<>();
        channelsSubscription = new ConcurrentHashMap<>();
        loginInformation = new ConcurrentHashMap<>();
        connectionIdToLogin = new ConcurrentHashMap<>();
        loginIsConnected = new ConcurrentHashMap<>();
    }

    public void subscribeToChannel(int connectionId, String channel) {
        channelsSubscription.putIfAbsent(channel, new ConcurrentLinkedQueue<>());
        channelsSubscription.get(channel).add(connectionId);
    }

    public void unSubscribeFromChannel(int connectionId, String channel) {
        if (channelsSubscription.containsKey(channel)) {
            channelsSubscription.get(channel).remove(connectionId);
        }
    }

    public boolean isSubscribed(int connectionId, String channel) {
        ConcurrentLinkedQueue<Integer> subscribes = channelsSubscription.get(channel);
        return channelsSubscription.get(channel) != null && subscribes.contains(connectionId);
    }

    public void addConnection(ConnectionHandler<T> connection, int connectionId) {
        connectionHandlers.put(connectionId, connection);
    }

    public String addLogin(String login, String passcode, int connectionId) {
        if (loginIsConnected.containsKey(login) && loginIsConnected.get(login)) {
            return "The client is already logged in, log out before trying again";
        }
        else if (loginIsConnected.containsKey(login) && !loginIsConnected.get(login) && !passcode.equals(loginInformation.get(login))) {
            return "wrong password";
        }
        else if (!loginIsConnected.containsKey(login)) {
            loginInformation.putIfAbsent(login, passcode);
            connectionIdToLogin.putIfAbsent(connectionId, login);
            loginIsConnected.putIfAbsent(login, true);
        }
        return null;
    }

    /**
     * TODO add message-id:
     * a server-unique id that for the message.
     * To be picked by the server.
     * maybe a global int
     */
    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> con = connectionHandlers.get(connectionId);
        if (con != null & msg != null) {
            System.out.println("Sending message to client:\n" + msg);
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
        loginIsConnected.put(connectionIdToLogin.get(connectionId), false);
        connectionIdToLogin.remove(connectionId);
        if (con != null) {
            for (String key : channelsSubscription.keySet()) {
                channelsSubscription.get(key).remove(connectionId);
            }
        }
    }
}
