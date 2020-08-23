import java.io.*;
import java.net.*;

public class HTTPClient {
    public static void main(String[] args) throws IOException {
        Socket s;
        byte[] buf = new byte[1024];

        /* Arg check */
        if (args.length < 1) {
            System.out.println("USAGE: java HTTPClient /<folder>/<filepath>");
            System.out.println("ex: java HTTPClient /test1/helo");
            return;
        }

        /* Connects to the server */
        s = new Socket("127.0.0.1", 8090);

        /* Writes the request */
        s.getOutputStream().write("GET ".getBytes(), 0, 4);
        s.getOutputStream().write(args[0].getBytes(), 0, args[0].length());
        s.getOutputStream().write("\r\n\r\n".getBytes(),  0, 4);

        /* Reads the entire response */
        for (int n; (n = s.getInputStream().read(buf)) > 0; )
            System.out.println(new String(buf, 0, n));

        s.close();
    }
}
