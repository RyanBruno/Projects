import java.io.*;
import java.net.*;

public class HTTPServer {

    public static String[] dirs = { "test1", "test2" };

    public static void main(String[] args) throws IOException {
        int port;
        ServerSocket ss;

        /* Parse the port */
        if (args.length < 1 || (port = parseInt(args[0])) < 0)
            port = 8090; // Default port number

        /* Open the server socket */
        ss = new ServerSocket(port); // Caught by caller

        System.out.println("Server listening on port: " + port);

        for (;;) {
            try {
                byte[] buf = new byte[1024];
                Socket cs;
                String[] req;
                InputStream file_in;

                /* Accept a socket */
                cs = ss.accept();

                /* Read the just the first line */
                req = new BufferedReader(
                        new InputStreamReader(
                            cs.getInputStream()))
                        .readLine().split(" ");
                 System.out.println(String.join(" ", req));

                /* Initial checks */
                if (req.length < 2 || !req[0].equals("GET") || req[1].charAt(0) != '/') {
                    cs.getOutputStream().write("400 Bad Request\r\n\r\n".getBytes(), 0, 19);
                    cs.close();
                    continue;
                }

                req[1] = req[1].substring(1);

                /* Check dirs list */
                if (!contains(dirs, req[1].split("/")[0])) {
                    cs.getOutputStream().write("403 Forbidden\r\n\r\n".getBytes(), 0, 17);
                    cs.close();
                    continue;
                }

                /* Try to open the file */
                try {
                    file_in = new FileInputStream(req[1]);
                } catch (FileNotFoundException e) {
                    cs.getOutputStream().write("404 Not Found\r\n\r\n".getBytes(), 0, 17);
                    cs.close();
                    continue;
                }

                /* File opened! */
                cs.getOutputStream().write("200 OK\r\n\r\n".getBytes(), 0, 10);

                /* Read the entire file and write all it to the client */
                for (int n; (n = file_in.read(buf)) > 0; )
                    cs.getOutputStream().write(buf, 0, n);

                cs.close();

            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }


    /* Helper functions */
    public static boolean contains(String[] hay, String needle) {
        for (String cur : hay)
            if (cur.equalsIgnoreCase(needle))
                return true;

        return false;
    }

    public static int parseInt(String str) {
        try {
            return Integer.parseInt(str);
        } catch (NumberFormatException e) {
            return -1;
        }
    }
}
