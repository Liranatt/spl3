package bgu.spl.net.impl.stomp.Frames;
import java.util.HashMap;

public abstract class Frame {
    protected String command;
    protected HashMap<String, String> headers;
    protected String body;

    public Frame(String command){
        this.command = command;
        this.headers = new HashMap<>();
    }
    public void addHeader(String key, String value){
        if (key != null && value != null)
            this.headers.put(key, value);
    }
    public void addBody(String body){
        this.body = body;
    }

//    public byte[] encode() {
//        StringBuilder output = new StringBuilder();
//        output.append(this.command + "\n");
//        for (String key: this.headers.keySet()){
//            String value = this.headers.get(key);
//            output.append(key+":"+value+"\n");
//        }
//        output.append("\n");
//        output.append(this.body + "\n");
//        output.append('\0');
//        return output.toString().getBytes();
//    }

    public String getCommand() {
        return command;
    }

    public String getBody() {
        return body;
    }

    public HashMap<String, String> getHeaders() {
        return headers;
    }
}
