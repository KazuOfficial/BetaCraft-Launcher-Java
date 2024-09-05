package com.johnymuffin.evolutions.core;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.nio.charset.Charset;
import java.util.HashMap;

import org.betacraft.launcher.Util;

import com.google.gson.JsonObject;

/*
 * A utility class (wrapper maybe?) for communicating with Beta Evolutions nodes
 *
 * @author RhysB
 * @version 1.0.3
 * @website https://evolutions.johnymuffin.com/
 *
 * This class has a license :)
 * ------------------------------------------------------------------------------
 * MIT License
 * Copyright (c) 2020 Rhys B
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ------------------------------------------------------------------------------
 *
 */

public class BetaEvolutionsUtils {
    private boolean debug;


    public BetaEvolutionsUtils() {
        debug = false;
    }

    public BetaEvolutionsUtils(boolean debug) {
        this.debug = debug;
    }

    //Core Methods - Start

    /**
     * Attempt to authenticate the user with Evolution nodes
     * !!!!This class is blocking
     *
     * @return VerificationResults class which contains successful/failed nodes
     */
    public VerificationResults authenticateUser(String username, String sessionID) {
        //Fetch IP address for V2 and above support
        String ip = getExternalIP();
        if (ip == null) {
            log("Can't authenticate with any nodes, can't fetch external IP address. Your internet is probably offline!");
            return new VerificationResults(0, 0, beServers.size());
        }
        VerificationResults verificationResults = new VerificationResults();
        //Iterate through all nodes while verifying
        for (String node : beServers.keySet()) {
            Boolean result = authenticateWithBetaEvolutions(username, node, beServers.get(node), sessionID, ip);
            if (result == null) {
                verificationResults.setErrored(verificationResults.getErrored() + 1);
            } else if (result == true) {
                verificationResults.setSuccessful(verificationResults.getSuccessful() + 1);
            } else if (result == false) {
                verificationResults.setFailed(verificationResults.getFailed() + 1);
            }
        }
        return verificationResults;

    }

    /**
     * Check if a user is authenticated with Evolution nodes
     * !!!!This class is blocking
     *
     * @return VerificationResults class which contains successful/failed nodes. Failed nodes mean an error occurred or the user wasn't verified. Successful means the user was verified/authenticated by the node.
     */
    public VerificationResults verifyUser(String username, String userIP) {
        VerificationResults verificationResults = new VerificationResults();
        //Iterate through all nodes while verifying
        for (String node : beServers.keySet()) {
            Boolean result = verifyUserWithNode(username, userIP, node, beServers.get(node));
            if (result == null) {
                verificationResults.setErrored(verificationResults.getErrored() + 1);
            } else if (result == true) {
                verificationResults.setSuccessful(verificationResults.getSuccessful() + 1);
            } else if (result == false) {
                verificationResults.setFailed(verificationResults.getFailed() + 1);
            }
        }
        return verificationResults;
    }


    //Core Methods - End

    //Server Methods - Start

    private Boolean verifyUserWithNode(String username, String userIP, String node, BEVersion beVersion) {
        //Return Types
        //True Successfully Authenticated
        //False Failed, probably didn't auth with that node
        //Null, an error occurred, probably internet outage
        if (beVersion == BEVersion.V1) {
            //Stage 1 - Contact node to confirm identification
            String stage1URL = node + "/serverAuth.php?method=1&username=" + encodeString(username) + "&userip=" + encodeString(userIP);
            JsonObject stage1Object = getJSONFromURL(stage1URL);
            if (stage1Object == null) {
                log("Authentication with node: " + node + " has failed to respond when queried.");
                return null;
            }
            if (!verifyJSONArguments(stage1Object, "result", "verified")) {
                log("Malformed response from: " + node + " using version " + beVersion);
                return null;
            }
            return stage1Object.get("verified").getAsBoolean();
        } else if (beVersion == BEVersion.V2_PLAINTEXT) {
            //Stage 1 - Contact node to confirm identification
            String stage1URL = node + "/server/getVerification?username=" + encodeString(username) + "&userip=" + encodeString(userIP);
            JsonObject stage1Object = getJSONFromURL(stage1URL);
            if (stage1Object == null) {
                log("Authentication with node: " + node + " has failed to respond when queried.");
                return null;
            }
            if (!verifyJSONArguments(stage1Object, "verified", "error")) {
                log("Malformed response from: " + node + " using version " + beVersion);
                return null;
            }
            return stage1Object.get("verified").getAsBoolean();
        }

        return null;
    }

    //Server Methods - End


    //Client Methods - Start
    private Boolean authenticateWithMojang(String username, String sessionID, String serverID) {
        try {
            String authURL = "http://session.minecraft.net/game/joinserver.jsp?user=" + encodeString(username) + "&sessionId=" + encodeString(sessionID) + "&serverId=" + encodeString(serverID);
            URL url = new URL(authURL);
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(url.openStream()));
            String response = bufferedReader.readLine();
            bufferedReader.close();
            if (response.equalsIgnoreCase("ok")) {
                return true;
            } else {
                return false;
            }
        } catch (Exception e) {
            if (debug) {
                log("An error occurred contacting Mojang.");
                e.printStackTrace();
            }
        }
        return null;
    }

    private Boolean authenticateWithBetaEvolutions(String username, String node, BEVersion beVersion, String sessionToken, String ip) {
        //Return Types
        //True Successfully Authenticated
        //False Failed, probably a cracked user
        //Null, an error occurred, probably internet outage
        if (beVersion == BEVersion.V1) {
            //State 1 - Contact the node with username and method type
            String stage1URL = node + "/userAuth.php?method=1&username=" + encodeString(username);
            JsonObject stage1Object = getJSONFromURL(stage1URL);
            if (stage1Object == null) {
                log("Authentication with node: " + node + " has failed as JSON can't be fetched.");
                return null;
            }
            if (!verifyJSONArguments(stage1Object, "result", "username", "userip", "serverId")) {
                log("Malformed response from: " + node + " using version " + beVersion);
                return null;
            }
            String serverID = stage1Object.get("serverId").getAsString();
            //Stage 2 - Contact Mojang to authenticate
            Boolean mojangAuthentication = authenticateWithMojang(username, sessionToken, serverID);
            if (mojangAuthentication == null) {
                log("Authentication with node: " + node + " has failed due to auth failure with Mojang.");
                return null;
            } else if (mojangAuthentication == false) {
                log("Authentication with node: " + node + " has failed. Token is probably incorrect, or user is cracked!");
                return false;
            }
            //Stage 3 - Contact node to confirm auth
            String stage3URL = node + "/userAuth.php?method=2&username=" + encodeString(username) + "&serverId=" + encodeString(serverID);
            JsonObject stage3Object = getJSONFromURL(stage3URL);
            if (stage3Object == null) {
                log("Authentication with node: " + node + " has failed as JSON can't be fetched.");
                return null;
            }
            if (!verifyJSONArguments(stage3Object, "result")) {
                log("Malformed response from: " + node + " using version " + beVersion);
                return null;
            }
            return stage3Object.get("result").getAsBoolean();


        } else if (beVersion == BEVersion.V2_PLAINTEXT) {
            //State 1 - Contact the node with username and method type
            String stage1URL = node + "/user/getServerID?username=" + encodeString(username) + "&userip=" + ip;
            JsonObject stage1Object = getJSONFromURL(stage1URL);
            if (stage1Object == null) {
                log("Authentication with node: " + node + " has failed as JSON can't be fetched.");
                return null;
            }
            if (!verifyJSONArguments(stage1Object, "userIP", "error", "serverID", "username")) {
                log("Malformed response from: " + node + " using version " + beVersion);
                return null;
            }
            String serverID = stage1Object.get("serverID").getAsString();
            //Stage 2 - Contact Mojang to authenticate
            Boolean mojangAuthentication = authenticateWithMojang(username, sessionToken, serverID);
            if (mojangAuthentication == null) {
                log("Authentication with node: " + node + " has failed due to auth failure with Mojang.");
                return null;
            } else if (mojangAuthentication == false) {
                log("Authentication with node: " + node + " has failed. Token is probably incorrect, or user is cracked!");
                return false;
            }
            //Stage 3 - Contact node to confirm auth
            String stage3URL = node + "/user/successfulAuth?username=" + encodeString(username) + "&serverid=" + encodeString(serverID) + "&userip=" + encodeString(ip);
            JsonObject stage3Object = getJSONFromURL(stage3URL);
            if (stage3Object == null) {
                log("Authentication with node: " + node + " has failed as JSON can't be fetched.");
                return null;
            }
            if (!verifyJSONArguments(stage3Object, "result")) {
                log("Malformed response from: " + node + " using version " + beVersion);
                return null;
            }
            return stage3Object.get("result").getAsBoolean();
        }
        return null;
    }

    public String getExternalIP() {
        String ip = getIPFromAmazon();
        if (ip == null) {
            ip = getIPFromWhatIsMyIpAddress();
        }
        return ip;
    }


    private String getIPFromAmazon() {
        try {
            URL myIP = new URL("http://checkip.amazonaws.com");
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(myIP.openStream()));
            return bufferedReader.readLine();

        } catch (Exception e) {
            log("Failed to get IP from Amazon, your internet is probably down.");
            if (debug) {
                e.printStackTrace();
            }
        }
        return null;
    }

    private String getIPFromWhatIsMyIpAddress() {
        try {
            URL myIP = new URL("https://ipv4bot.whatismyipaddress.com/");
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(myIP.openStream()));
            return bufferedReader.readLine();

        } catch (Exception e) {
            log("Failed to get IP from WhatIsMyIpAddress, your internet is probably down.");
            if (debug) {
                e.printStackTrace();
            }
        }
        return null;
    }


    //Client Methods - End

    /*Node Storage - Start
     */
    private static HashMap<String, BEVersion> beServers = new HashMap<String, BEVersion>();

    static {
        //Some nodes may support multiple types. Ideally in the future, this class will contact nodes asking for their protocol versions, however, V2 should remain online.
//        beServers.put("https://auth.johnymuffin.com", BEVersion.V1);
        beServers.put("https://auth1.evolutions.johnymuffin.com", BEVersion.V2_PLAINTEXT);
        beServers.put("https://auth2.evolutions.johnymuffin.com", BEVersion.V2_PLAINTEXT);
        beServers.put("https://auth3.evolutions.johnymuffin.com", BEVersion.V2_PLAINTEXT);
    }

    public enum BEVersion {
        V1,
        V2_PLAINTEXT,
    }

    //Node Storage - Start

    //Utils - Start

    //Method readJsonFromUrl and readAll licensed under CC BY-SA 2.5 (https://stackoverflow.com/help/licensing)
    //Credit: https://stackoverflow.com/a/4308662

    private static JsonObject readJsonFromUrl(String url) throws IOException {
//        InputStream is = (new URL(url)).openStream();
//        try {
//            BufferedReader rd = new BufferedReader(new InputStreamReader(is, Charset.forName("UTF-8")));
//            String jsonText = readAll(rd);
//            JSONObject json = new JSONObject(jsonText);
//            return json;
//        } finally {
//            is.close();
//        }
        return readJsonFromUrlWithTimeout(url, 5000);
    }

    private static JsonObject readJsonFromUrlWithTimeout(String url, int timeout) throws IOException {
        URL myURL = new URL(url);
        HttpURLConnection connection = (HttpURLConnection) myURL.openConnection();
        connection.setConnectTimeout(timeout);
        connection.setReadTimeout(timeout);
        connection.setRequestMethod("GET");
        connection.connect();
        InputStream is = connection.getInputStream();
        try {
            BufferedReader rd = new BufferedReader(new InputStreamReader(is, Charset.forName("UTF-8")));
            String jsonText = readAll(rd);
            JsonObject json = Util.gson.fromJson(jsonText, JsonObject.class);
            return json;
        } finally {
            is.close();
        }
    }

    private static String readAll(Reader rd) throws IOException {
        StringBuilder sb = new StringBuilder();
        int cp;
        while ((cp = rd.read()) != -1)
            sb.append((char) cp);
        return sb.toString();
    }


    private JsonObject getJSONFromURL(String url) {
        try {
        	JsonObject jsonObject = readJsonFromUrl(url);
            return jsonObject;
        } catch (Exception e) {
            if (debug) {
                log("An error occurred fetching JSON from: " + url);
                e.printStackTrace();
            }
        }
        return null;
    }


    private void log(String info) {
        if (debug) {
            System.out.println("[Beta Evolutions] " + info);
        }
    }

    private String encodeString(String string) {
        try {
            return URLEncoder.encode(string, "UTF-8");
        } catch (Exception e) {
            log("An error occurred encoding a string, this really shouldn't happen on modern JVMs.");
            e.printStackTrace();
        }
        return null;
    }


    private boolean verifyJSONArguments(JsonObject jsonObject, String... arguments) {
        for (String s : arguments) {
            if (!jsonObject.has(s)) return false;
        }
        return true;
    }


    //Utils - End


    public class VerificationResults {
        private int successful = 0;
        private int failed = 0;
        private int errored = 0;

        public VerificationResults() {
        }

        public VerificationResults(int successful, int failed, int errored) {
            this.successful = successful;
            this.failed = failed;
            this.errored = errored;
        }


        public int getSuccessful() {
            return successful;
        }

        public void setSuccessful(int successful) {
            this.successful = successful;
        }

        public int getFailed() {
            return failed;
        }

        public void setFailed(int failed) {
            this.failed = failed;
        }

        public int getErrored() {
            return errored;
        }

        public void setErrored(int errored) {
            this.errored = errored;
        }

        public int getTotal() {
            return (errored + successful + failed);
        }

    }


}