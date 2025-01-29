package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;


public class StompServer {

    public static void main(String[] args) {
        Server.threadPerClient(
                        7777,
                        StompMessagingProtocolImpl::new,
                        StompMessageEncoderDecoder::new
                ).serve();

        // if (args != null && args.length == 2 && args[0] != null & args[1] != null) {
        //     int port = Integer.parseInt(args[0]);

        //     if (args[1].equals("tpc")) {
        //         Server.threadPerClient(
        //                 port,
        //                 StompMessagingProtocolImpl::new,
        //                 StompMessageEncoderDecoder::new
        //         ).serve();
        //     }

        //     else if (args[1].equals("reactor")) {
        //         Server.reactor(
        //                 Runtime.getRuntime().availableProcessors(),
        //                 port,
        //                 StompMessagingProtocolImpl::new,
        //                 StompMessageEncoderDecoder::new
        //         ).serve();
        //     }

        //     // mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="7777 tpc"
        //     // mvn exec:java -Dexec.mainClass="bgu.spl.net.impl.stomp.StompServer" -Dexec.args="7777 reactor"
        // }
        // else {
        //     System.out.println("Illegel arguments !");
        // }
    }
}
