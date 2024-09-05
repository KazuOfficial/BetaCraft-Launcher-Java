package uk.betacraft.auth.jsons.mojang.session;

import java.security.MessageDigest;

import uk.betacraft.auth.BlankResponse;
import uk.betacraft.auth.Request;
import uk.betacraft.auth.RequestUtil;
import uk.betacraft.auth.Response;
import uk.betacraft.util.WebData;

public class JoinServerRequest extends Request {

	public String accessToken;
	public String selectedProfile;
	public String serverId;

	public JoinServerRequest(String sessionid, String uuid, String serverSocket) {
		this.REQUEST_URL = "https://sessionserver.mojang.com/session/minecraft/join";
		this.PROPERTIES.put("Content-Type", "application/json");
		this.serverId = sha1(serverSocket);
		this.accessToken = sessionid;
		this.selectedProfile = uuid;
	}

	@Override
	public JoinServerResponse perform() {
		return new JoinServerResponse(RequestUtil.performRawPOSTRequest(this));
	}

	public static String sha1(String input) {
		try {
			MessageDigest mDigest = MessageDigest.getInstance("SHA-1");
			byte[] result = mDigest.digest(input.getBytes());
			StringBuffer sb = new StringBuffer();
			for (int i = 0; i < result.length; ++i) {
				sb.append(Integer.toString((result[i] & 0xFF) + 256, 16).substring(1));
			}
			return sb.toString();
		} catch (Throwable t) {
			t.printStackTrace();
			return null;
		}
	}

	public static class JoinServerResponse extends Response {
		private WebData data;

		public JoinServerResponse(WebData data) {
			this.data = data;
		}

		public WebData getData() {
			return this.data;
		}
	}
}
