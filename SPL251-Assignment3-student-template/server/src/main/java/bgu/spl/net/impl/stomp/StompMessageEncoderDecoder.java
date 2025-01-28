package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.impl.stomp.Frames.*;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

public class StompMessageEncoderDecoder implements MessageEncoderDecoder<Frame> {

    private byte[] bytes = new byte[1 << 10]; //start with 1k
    private int len = 0;

    @Override
    public Frame decodeNextByte(byte nextByte) {
        if (nextByte == '\0') {
            return parseToFrame();
        }

        pushByte(nextByte);
        return null; //not a line yet
    }

    @Override
    public byte[] encode(Frame message) {
        StringBuilder output = new StringBuilder();
        output.append(message.getCommand() + "\n");
        for (String key: message.getHeaders().keySet()){
            String value = message.getHeaders().get(key);
            output.append(key+":"+value+"\n");
        }
        output.append("\n");
        output.append(message.getBody() + "\n");
        output.append('\0');
        return output.toString().getBytes();
    }



    private void pushByte(byte nextByte) {
        if (len >= bytes.length) {
            bytes = Arrays.copyOf(bytes, len * 2);
        }

        bytes[len++] = nextByte;
    }

    private Frame parseToFrame() {
        String message = popString();
        Frame result = null;
        if (message != null && message.length() > 0) {
            String command = message.substring(0,message.indexOf('\n'));
            message = message.substring(message.indexOf('\n') + 1);
            switch (command) {
                case "CONNECT":
                    result = new ConnectFrame();
                    break;
                case "SEND":
                    result = new SendFrame();
                    break;
                case "SUBSCRIBE":
                    result = new SubscribeFrame();
                    break;
                case "UNSUBSCRIBE":
                    result = new UnSubscribeFrame();
                    break;
                case "DISCONNECT":
                    result = new DisconnectFrame();
                    break;
            }
            if (result != null) {
                while (message.indexOf('\n') != 0) {
                    String line = message.substring(0, message.indexOf('\n'));
                    result.addHeader(line.substring(0,line.indexOf(':')), line.substring(line.indexOf(':') + 1));
                    message = message.substring(message.indexOf('\n') + 1);
                }
                if (message.indexOf('\0') == -1)
                    result.addBody(message.substring(1));
                else
                    result.addBody(message.substring(1, message.length()-1));
            }
        }
        return result;
    }

    private String popString() {
        //notice that we explicitly requesting that the string will be decoded from UTF-8
        //this is not actually required as it is the default encoding in java.
        String result = new String(bytes, 0, len, StandardCharsets.UTF_8);
        len = 0;
        return result;
    }
}
