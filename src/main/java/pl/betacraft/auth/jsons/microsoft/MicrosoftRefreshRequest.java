package pl.betacraft.auth.jsons.microsoft;

import com.google.gson.Gson;
import com.google.gson.JsonParseException;

import pl.betacraft.auth.MicrosoftAuth;
import pl.betacraft.auth.Request;
import pl.betacraft.auth.RequestUtil;

public class MicrosoftRefreshRequest extends Request {

	public MicrosoftRefreshRequest(String refresh_token) {
		REQUEST_URL = "https://login.live.com/oauth20_token.srf";
		POST_DATA = "client_id=" + MicrosoftAuth.CLIENT_ID +
				"&refresh_token=" + refresh_token +
				"&grant_type=refresh_token" +
				"&redirect_uri=" + MicrosoftAuth.REDIRECT_URI +
				"&scope=service::user.auth.xboxlive.com::MBI_SSL";
		PROPERTIES.put("Content-Type", "application/x-www-form-urlencoded");
	}

	@Override
	public MicrosoftAuthResponse perform() {
		Gson gson = new Gson();
		String response = RequestUtil.performPOSTRequest(this);

		MicrosoftAuthResponse ret;
		try {
			ret = gson.fromJson(response, MicrosoftAuthResponse.class);
		} catch (JsonParseException ex) {
			return null;
		}
		return ret;
	}
}