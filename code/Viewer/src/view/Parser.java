package view;

import javafx.scene.control.TextArea;

public class Parser {
    Window window;
    Client client;

    public Parser(Window w, TextArea t) {
        window = w;
        window.display = t;
        client = new Client(window);
    }

    private boolean ModelisValid(String m) {
        if (m.contentEquals(new String("RandomWayPoint")) || m.contentEquals(new String("VerticalPathWay"))
                || m.contentEquals(new String("HorizontalPathWay")))
            return true;
        return false;
    }

    synchronized public void parser(String flux) {
        String[] string = flux.split(" |\\, ");
        switch (string[0]) {
            case "hello":
                if (string.length == 1
                        || (string.length == 4 && string[1].contentEquals("in") && string[2].contentEquals("as"))) {
                    client.send(flux);
                } else
                    window.display.appendText(
                            "< NOK. Invalid number of arguments (expected 0 or 3) " + System.lineSeparator());
                break;
            case "status":
                if (string.length == 1) {
                    if (client.isConnected()) {
                        window.display.appendText("< OK : Connected, " + window.view.toString());
                    } else
                        window.display.appendText("< NOK : Not connected." + window.view.toString());
                } else
                    window.display
                            .appendText("< NOK. Invalid number of arguments (expected 0)" + System.lineSeparator());
                break;
            case "log":
                if (string.length == 2) {
                    if (string[1].contentEquals("out")) {
                        client.send(flux);
                    } else
                        window.display.appendText("< NOK. Invalid argument (expected 'out') " + System.lineSeparator());
                } else
                    window.display.appendText("< NOK. Invalid number of arguments  " + System.lineSeparator());
                break;

            case "addFish":
                if (string.length == 6) {
                    if (!ModelisValid(string[5]))
                        window.display.appendText("< NOK : Model not found." + System.lineSeparator());
                    else {
                        client.handler.History.add(flux);
                        client.send(flux);
                    }
                } else

                    window.display
                            .appendText("< NOK. Invalid number of arguments (expected 5)" + System.lineSeparator());
                break;
            case "startFish":
                if (string.length == 2) {
                    if (window.view.hasFish(string[1])) {
                        client.handler.History.add(flux);
                        client.send(flux);
                    } else
                        window.display.appendText("< NOK : Fish doesn't exist" + System.lineSeparator());
                } else
                    window.display
                            .appendText("< NOK. Invalid number of arguments (expected 1)" + System.lineSeparator());
                break;
            case "delFish":
                if (string.length == 2) {
                    if (window.view.hasFish(string[1])) {
                        client.handler.History.add(flux);
                        client.send(flux);
                    } else
                        window.display.appendText("< NOK : Fish doesn't exist" + System.lineSeparator());
                } else
                    window.display
                            .appendText("< NOK. Invalid number of arguments (expected 1)" + System.lineSeparator());
                break;
            case "getFishes":
                if (string.length == 1) {
                    client.send(flux);
                } else
                    window.display
                            .appendText("< NOK. Invalid number of arguments (expected 0)" + System.lineSeparator());
                break;
            case "getFishesContinuously":
                if (string.length == 1) {
                    client.send(flux);
                } else
                    window.display
                            .appendText("< NOK. Invalid number of arguments (expected 0)" + System.lineSeparator());
                break;
            default:
                window.display.appendText("< NOK. Command Not Found" + System.lineSeparator());
                break;
        }
    }
}