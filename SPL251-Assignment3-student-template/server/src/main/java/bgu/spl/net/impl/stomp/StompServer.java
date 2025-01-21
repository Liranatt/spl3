package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocolImpl;
import bgu.spl.net.api.StomptMessageEncoderDecoder;
import bgu.spl.net.impl.echo.EchoProtocol;
import bgu.spl.net.impl.echo.LineMessageEncoderDecoder;
import bgu.spl.net.srv.Server;
import bgu.spl.net.impl.stomp.Frame;


public class StompServer {

    public static void main(String[] args) {

        // you can use any server...
        Server.threadPerClient(
                7777, //port
                StompMessagingProtocolImpl::new, //protocol factory
                StomptMessageEncoderDecoder::new //message encoder decoder factory
        ).serve();

         Server.reactor(
                 Runtime.getRuntime().availableProcessors(),
                 7777, //port
                 StompMessagingProtocolImpl::new, //protocol factory
                 StomptMessageEncoderDecoder::new //message encoder decoder factory
         ).serve();
        // TODO: implement this
    }
}
