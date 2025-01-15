package bgu.spl.net.impl.stomp;

public class connect extends Frame {
    public connect(String username, String password){
        super("CONNECT");
        this.addHeader("login", username);
        this.addHeader("passcode", password);
        this.addHeader("accept-version", "1.2");
        this.addHeader("host", "stomp.cs.bgu.ac.il");
    }
}
