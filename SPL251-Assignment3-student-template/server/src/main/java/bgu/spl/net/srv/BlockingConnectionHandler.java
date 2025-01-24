package bgu.spl.net.srv;

import bgu.spl.net.api.StompMessagingProtocolImpl;
import bgu.spl.net.api.StompMessageEncoderDecoder;
import bgu.spl.net.impl.stomp.Frames.ErrorFrame;
import bgu.spl.net.impl.stomp.Frames.Frame;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;

public class BlockingConnectionHandler<T> implements Runnable, ConnectionHandler<T> {

    private final StompMessagingProtocolImpl protocol;
    private final StompMessageEncoderDecoder encdec;
    private final Socket sock;
    private BufferedInputStream in;
    private BufferedOutputStream out;
    private volatile boolean connected = true;

    public BlockingConnectionHandler(Socket sock, StompMessageEncoderDecoder reader, StompMessagingProtocolImpl protocol, int clientId) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = protocol;
        protocol.start(clientId, ConnectionsImpl.getInstance());
    }

    @Override
    public void run() {
        try (Socket sock = this.sock) { //just for automatic closing
            int read;

            in = new BufferedInputStream(sock.getInputStream());
            out = new BufferedOutputStream(sock.getOutputStream());

            while (!protocol.shouldTerminate() && connected && (read = in.read()) >= 0) {
                Frame nextMessage = encdec.decodeNextByte((byte) read);
                if (nextMessage != null) {
                    protocol.process(nextMessage);
//                    T response = protocol.process(nextMessage);
//                    if (response != null) {
//                        out.write(encdec.encode(response));
//                        out.flush();
//                    }
                }
            }

        } catch (IOException ex) {
            ex.printStackTrace();
        }

    }

    @Override
    public void close() throws IOException {
        connected = false;
        sock.close();
    }

    @Override
    public void send(T msg) {
        if (msg != null & connected) {
            try {
                synchronized (out) {
                    out.write(encdec.encode((Frame) msg));
                    out.flush();
                }
            } catch (IOException e) {
                System.out.println(e.getMessage());
            }
            if (msg instanceof ErrorFrame) {
                try {
                    close();
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
        }
    }
}
