package view;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;


// Classe qui communique avec le socket en utilisant TCP
public class Client {

    private Socket sock;
    protected BufferedReader in;
    protected Thread receiver;
    protected Handler handler;

    private Window window;


    // Construteur.
    public Client(Window w) {
        window = w;
    }


    // Roivoie si le socket est connecté.
    public boolean isConnected() {
        return (sock != null && sock.isConnected());
    }


    // Configuration de la connexion.
    public void configurate(String ipAddress, int port) {
        boolean isConnected = false;
        while (!isConnected) {
            try {
                window.writeLogs("Connexion attempt with server of IPadress : "
                        + InetAddress.getByName(InetAddress.getLocalHost().toString().split("/")[1]) + " in Port n° :"
                        + port + "\n");

                sock = new Socket(ipAddress, port);
                in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
                isConnected = true;
            } catch (IOException exception) {
                isConnected = false;
            }
        }
        window.writeLogs("Connexion established successfully \n");
        handler = new Handler(window, in);
        receiver = new Thread(handler);
        receiver.start();
    }


    // Fermer la connexion.
    synchronized public void logOut() {
        try {
            if (sock != null) {
                sock.close();
            }
        } catch (IOException exception) {
            System.out.println("Exception : " + exception.toString());
            window.writeLogs("Exception while closing the socket\n");
        }
    }

   
    // Envoyer un message "s" sur la socket au serveur.
    public void send(String s) {
        window.writeLogs("Sending to the server : " + s + "\n");
        if (isConnected()) {
            try {
                PrintWriter out = new PrintWriter(sock.getOutputStream());
                window.view.timeGone = 0;
                out.println(s);
                out.flush();
            } catch (IOException exception) {
                System.out.println("Exception : send "+ exception.toString());
                window.writeLogs("Exception while sending.");
            }
        }
    }

}
