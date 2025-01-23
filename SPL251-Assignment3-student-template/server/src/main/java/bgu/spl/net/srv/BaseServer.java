package bgu.spl.net.srv;

import bgu.spl.net.api.StompMessagingProtocolImpl;
import bgu.spl.net.api.StompMessageEncoderDecoder;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.function.Supplier;

public abstract class BaseServer<T> implements Server<T> {

    private final int port;
    private final Supplier<StompMessagingProtocolImpl> protocolFactory;
    private final Supplier<StompMessageEncoderDecoder> encdecFactory;
    private final ConnectionsImpl<T> connections;
    private ServerSocket sock;
    private int numClients;

    public BaseServer(
            int port,
            Supplier<StompMessagingProtocolImpl> protocolFactory,
            Supplier<StompMessageEncoderDecoder> encdecFactory) {

        this.port = port;
        this.protocolFactory = protocolFactory;
        this.encdecFactory = encdecFactory;
		this.sock = null;
		this.connections = ConnectionsImpl.getInstance();
        numClients = 0;
    }

    @Override
    public void serve() {

        try (ServerSocket serverSock = new ServerSocket(port)) {
			System.out.println("Server started liran and nir rulez");

            this.sock = serverSock; //just to be able to close

            while (!Thread.currentThread().isInterrupted()) {

                Socket clientSock = serverSock.accept();


                BlockingConnectionHandler<T> handler = new BlockingConnectionHandler<>(
                        clientSock,
                        encdecFactory.get(),
                        protocolFactory.get(),
                        numClients);

                connections.addConnection(handler, numClients);
                numClients++;
                execute(handler);
            }
        } catch (IOException ex) {
        }

        System.out.println("server closed!!!");
    }

    @Override
    public void close() throws IOException {
		if (sock != null)
			sock.close();
    }

    protected abstract void execute(BlockingConnectionHandler<T>  handler);

}
