package bgu.spl.net.impl.stomp.Frames;

public class ConnectedFrame extends Frame {

    public ConnectedFrame() {
        super("CONNECTED");
        addHeader("version", "1.2");
    }
}
