package bgu.spl.net.impl.stomp.Frames;

public class ConnectFrame extends Frame {

    /// client constructor
    public ConnectFrame(String username, String password) {
        super("CONNECT");
        this.addHeader("login", username);
        this.addHeader("passcode", password);
        this.addHeader("accept-version", "1.2");
        this.addHeader("host", "stomp.cs.bgu.ac.il");
    }

    /// server constructor
    public ConnectFrame() {
        super("CONNECT");
    }
}
