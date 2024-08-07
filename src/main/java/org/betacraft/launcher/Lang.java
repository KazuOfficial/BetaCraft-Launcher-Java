package org.betacraft.launcher;

import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListSelectionModel;
import javax.swing.UIManager;

import org.betacraft.launcher.Util.PropertyFile;

public class Lang extends JFrame implements LanguageElement {
	public static List<String> locales = new ArrayList<String>();

	static JList list;
	static DefaultListModel listModel;
	static JScrollPane listScroller;
	JButton OKButton;
	static JPanel panel;
	static GridBagConstraints constr;

	public static String locale_id = "1.09_17";

	public Lang() {
		System.out.println("Language selection window opened.");
		this.setIconImage(Window.img);
		this.setMinimumSize(new Dimension(282, 386));

		this.setTitle(Lang.LANG);
		this.setResizable(true);
		final boolean panelnull = panel == null;

		if (panelnull) {
			panel = new JPanel();
			panel.setLayout(new GridBagLayout());
		}

		constr = new GridBagConstraints();

		constr.gridx = 0;
		constr.fill = GridBagConstraints.BOTH;
		constr.insets = new Insets(5, 5, 0, 5);
		constr.gridwidth = GridBagConstraints.RELATIVE;
		constr.gridheight = GridBagConstraints.RELATIVE;
		constr.gridy = 0;
		constr.weightx = 1.0;
		constr.weighty = 1.0;
		try {
			if (locales.isEmpty()) initLang();
		} catch (IOException e1) {
			e1.printStackTrace();
		}

		constr.gridy++;
		constr.weighty = GridBagConstraints.RELATIVE;
		constr.gridheight = 1;
		constr.insets = new Insets(5, 5, 5, 5);
		if (panelnull) {
			OKButton = new JButton(Lang.OPTIONS_OK);
			OKButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					setLang();
					Window.lang = null;
				}
			});
			panel.add(OKButton, constr);
		}
		this.add(panel);
		this.pack();
		this.setLocationRelativeTo(Window.mainWindow);
		this.setVisible(true);
	}

	public void update() {
		this.setTitle(Lang.LANG);
		OKButton.setText(Lang.OPTIONS_OK);
	}

	public void setLang() {
		String lang = (String) list.getSelectedValue();

		// Only return if we failed to download without a backup
		if (!download(lang).isPositive()) {
			return;
		}
		BC.SETTINGS.setProperty("language", lang);
		BC.SETTINGS.flushToDisk();
		dispose();
		Launcher.restart(Launcher.javaRuntime.getAbsolutePath());
	}

	public void initLang() throws IOException {
		URL url = new URL("http://files.betacraft.uk/launcher/assets/lang/" + locale_id + "/list.txt");

		Scanner scanner = new Scanner(url.openStream(), "UTF-8");
		String now;
		while (scanner.hasNextLine()) {
			now = scanner.nextLine();
			if (now.equalsIgnoreCase("")) continue;
			locales.add(now);
		}
		scanner.close();

		int i = 0;
		int index = 0;
		listModel = new DefaultListModel();
		String lang = BC.SETTINGS.getProperty("language");
		for (String item : locales) {
			listModel.addElement(item);
			if (lang.equals(item)) {
				index = i;
			}
			i++;
		}


		list = new JList(listModel);
		list.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		list.setLayoutOrientation(JList.VERTICAL);
		list.setVisibleRowCount(10);
		list.setSelectedIndex(index);

		if (listScroller != null) panel.remove(listScroller);

		listScroller = new JScrollPane(list);
		listScroller.setWheelScrollingEnabled(true);
		panel.add(listScroller, constr);
	}

	public static void applyNamesSwing() {
		try {
			UIManager.put("OptionPane.okButtonText", OPTIONS_OK);
			UIManager.put("OptionPane.cancelButtonText", CANCEL);
			UIManager.put("OptionPane.yesButtonText", YES);
			UIManager.put("OptionPane.noButtonText", NO);
		} catch (Throwable t) {
			t.printStackTrace();
		}
	}

	public static boolean downloaded(String lang) {
		File file = new File(BC.get() + "launcher" + File.separator + "lang" + File.separator + lang + ".txt");
		if (file.exists()) {
			return true;
		}
		return false;
	}

	public static String encodeForURL(String string) {
		try {
			return URLEncoder.encode(string, "UTF-8").replace("+", "%20"); // java moment
		} catch (Throwable t) {
			t.printStackTrace();
			return null;
		} // impossible unless null
	}

	public static DownloadResult download(String lang) {
		DownloadResult download = Launcher.download("http://files.betacraft.uk/launcher/assets/lang/" + locale_id + "/" + encodeForURL(lang) + ".txt", new File(BC.get() + "launcher" + File.separator + "lang", lang + ".txt"));
		if (!download.isPositive()) {
			JOptionPane.showMessageDialog(Window.mainWindow, "No Internet connection", "Language file download failed!", JOptionPane.ERROR_MESSAGE);
		}
		return download;
	}

	public static void refresh(boolean download, boolean force) {
		String lang = BC.SETTINGS.getProperty("language");
		if (lang == null) {
			applyNamesSwing();
			return;
		}
		File file = new File(BC.get() + "launcher" + File.separator + "lang" + File.separator +  lang + ".txt");
		if ((download && !download(lang).isOK()) || !file.exists()) {
			applyNamesSwing();
			return;
		}
		String charset = "UTF-8";

		PropertyFile langentries = new PropertyFile(file, charset);

		if (langentries.hasProperty("version_button"))
			WINDOW_SELECT_VERSION = langentries.getProperty("version_button");

		if (langentries.hasProperty("play_button"))
			WINDOW_PLAY = langentries.getProperty("play_button");

		if (langentries.hasProperty("options_button"))
			WINDOW_OPTIONS = langentries.getProperty("options_button");

		if (langentries.hasProperty("launcher_title"))
			WINDOW_TITLE = langentries.getProperty("launcher_title") + Launcher.VERSION;

		if (langentries.hasProperty("language"))
			WINDOW_LANGUAGE = langentries.getProperty("language");

		if (langentries.hasProperty("downloading"))
			WINDOW_DOWNLOADING = langentries.getProperty("downloading");

		if (langentries.hasProperty("downloading_resource"))
			WINDOW_DOWNLOADING_RESOURCE = langentries.getProperty("downloading_resource");

		if (langentries.hasProperty("packing_mod"))
			PACKING_MOD = langentries.getProperty("packing_mod");

		if (langentries.hasProperty("manual_download"))
			MANUAL_DOWNLOAD = langentries.getProperty("manual_download");

		if (langentries.hasProperty("username_field_empty"))
			WINDOW_USERNAME_FIELD_EMPTY = langentries.getProperty("username_field_empty");


		if (langentries.hasProperty("lang_title"))
			LANG = langentries.getProperty("lang_title");


		if (langentries.hasProperty("unexpected_error"))
			UNEXPECTED_ERROR = langentries.getProperty("unexpected_error");

		if (langentries.hasProperty("no_connection"))
			ERR_NO_CONNECTION = langentries.getProperty("no_connection");

		if (langentries.hasProperty("download_fail"))
			ERR_DL_FAIL = langentries.getProperty("download_fail");


		if (langentries.hasProperty("update_check"))
			OPTIONS_UPDATE_HEADER = langentries.getProperty("update_check");

		if (langentries.hasProperty("use_betacraft"))
			OPTIONS_PROXY = langentries.getProperty("use_betacraft");

		if (langentries.hasProperty("discord_rpc"))
			OPTIONS_RPC = langentries.getProperty("discord_rpc");

		if (langentries.hasProperty("launch_arguments"))
			OPTIONS_LAUNCH_ARGS = langentries.getProperty("launch_arguments") + ":";

		if (langentries.hasProperty("keep_launcher_open"))
			OPTIONS_KEEP_OPEN = langentries.getProperty("keep_launcher_open");

		if (langentries.hasProperty("width"))
			OPTIONS_WIDTH = langentries.getProperty("width");

		if (langentries.hasProperty("height"))
			OPTIONS_HEIGHT = langentries.getProperty("height");

		if (langentries.hasProperty("options_title"))
			OPTIONS_TITLE = langentries.getProperty("options_title");

		if (langentries.hasProperty("ok"))
			OPTIONS_OK = langentries.getProperty("ok");


		if (langentries.hasProperty("sort_oldest"))
			SORT_FROM_OLDEST = langentries.getProperty("sort_oldest");

		if (langentries.hasProperty("sort_newest"))
			SORT_FROM_NEWEST = langentries.getProperty("sort_newest");

		if (langentries.hasProperty("version_title"))
			VERSION_LIST_TITLE = langentries.getProperty("version_title");


		if (langentries.hasProperty("addon_list_title"))
			ADDON_LIST_TITLE = langentries.getProperty("addon_list_title");

		if (langentries.hasProperty("addon_no_desc"))
			ADDON_NO_DESC = langentries.getProperty("addon_no_desc");

		if (langentries.hasProperty("addon_show_info"))
			ADDON_SHOW_INFO = langentries.getProperty("addon_show_info");


		if (langentries.hasProperty("logging_in"))
			LOGGING_IN = langentries.getProperty("logging_in");

		if (langentries.hasProperty("login_title"))
			LOGIN_TITLE = langentries.getProperty("login_title");

		if (langentries.hasProperty("log_in_button"))
			LOGIN_BUTTON = langentries.getProperty("log_in_button");

		if (langentries.hasProperty("log_out_button"))
			LOGOUT_BUTTON = langentries.getProperty("log_out_button");

		if (langentries.hasProperty("login_email_nickname"))
			LOGIN_EMAIL_NICKNAME = langentries.getProperty("login_email_nickname");

		if (langentries.hasProperty("login_password"))
			LOGIN_PASSWORD = langentries.getProperty("login_password");

		if (langentries.hasProperty("login_mojang_header"))
			LOGIN_MOJANG_HEADER = langentries.getProperty("login_mojang_header");

		if (langentries.hasProperty("login_microsoft_button"))
			LOGIN_MICROSOFT_BUTTON = langentries.getProperty("login_microsoft_button");

		if (langentries.hasProperty("login_microsoft_title"))
			LOGIN_MICROSOFT_TITLE = langentries.getProperty("login_microsoft_title");

		if (langentries.hasProperty("login_microsoft_error"))
			LOGIN_MICROSOFT_ERROR = langentries.getProperty("login_microsoft_error");

		if (langentries.hasProperty("login_microsoft_parent"))
			LOGIN_MICROSOFT_PARENT = langentries.getProperty("login_microsoft_parent");

		if (langentries.hasProperty("login_microsoft_no_xbox"))
			LOGIN_MICROSOFT_NO_XBOX = langentries.getProperty("login_microsoft_no_xbox");

		if (langentries.hasProperty("login_microsoft_no_minecraft"))
			LOGIN_MICROSOFT_NO_MINECRAFT = langentries.getProperty("login_microsoft_no_minecraft");

		if (langentries.hasProperty("login_microsoft_code_line1"))
			LOGIN_MICROSOFT_CODE_LINE1 = langentries.getProperty("login_microsoft_code_line1");

		if (langentries.hasProperty("login_microsoft_code_line2"))
			LOGIN_MICROSOFT_CODE_LINE2 = langentries.getProperty("login_microsoft_code_line2");
		
		if (langentries.hasProperty("login_relogin"))
			LOGIN_RELOGIN = langentries.getProperty("login_relogin");


		if (langentries.hasProperty("login_failed"))
			LOGIN_FAILED = langentries.getProperty("login_failed");

		if (langentries.hasProperty("login_failed_invalid_credentials"))
			LOGIN_FAILED_INVALID_CREDENTIALS = langentries.getProperty("login_failed_invalid_credentials");


		if (langentries.hasProperty("java_executable"))
			JAVA_EXECUTABLE = langentries.getProperty("java_executable");

		if (langentries.hasProperty("java_invalid"))
			JAVA_INVALID = langentries.getProperty("java_invalid");

		if (langentries.hasProperty("java_too_recent"))
			JAVA_TOO_RECENT = langentries.getProperty("java_too_recent");

		if (langentries.hasProperty("java_recommended_not_found"))
			JAVA_RECOMMENDED_NOT_FOUND = langentries.getProperty("java_recommended_not_found");

		if (langentries.hasProperty("java_wrong_arch"))
			JAVA_WRONG_ARCH = langentries.getProperty("java_wrong_arch");

		if (langentries.hasProperty("java_ssl_not_supported"))
			JAVA_SSL_NOT_SUPPORTED = langentries.getProperty("java_ssl_not_supported");

		if (langentries.hasProperty("java_ssl_to_download_resource"))
			JAVA_SSL_TO_DOWNLOAD_RESOURCE = langentries.getProperty("java_ssl_to_download_resource");

		if (langentries.hasProperty("java_ssl_to_microsoft_account"))
			JAVA_SSL_TO_MICROSOFT_ACCOUNT = langentries.getProperty("java_ssl_to_microsoft_account");

		if (langentries.hasProperty("java_intel_graphics"))
			JAVA_INTEL_GRAPHICS = langentries.getProperty("java_intel_graphics");

		if (langentries.hasProperty("java_intel_graphics_wiki"))
			JAVA_INTEL_GRAPHICS_WIKI = langentries.getProperty("java_intel_graphics_wiki");


		if (langentries.hasProperty("instance_directory"))
			INSTANCE_DIRECTORY = langentries.getProperty("instance_directory");

		if (langentries.hasProperty("instance_remove_directory"))
			INSTANCE_REMOVE_DIRECTORY = langentries.getProperty("instance_remove_directory");

		if (langentries.hasProperty("instance_game_directory_title"))
			INSTANCE_GAME_DIRECTORY_TITLE = langentries.getProperty("instance_game_directory_title");

		if (langentries.hasProperty("instance_remove_question"))
			INSTANCE_REMOVE_QUESTION = langentries.getProperty("instance_remove_question");

		if (langentries.hasProperty("instance_remove_title"))
			INSTANCE_REMOVE_TITLE = langentries.getProperty("instance_remove_title");

		if (langentries.hasProperty("instance_change_icon_name"))
			INSTANCE_CHANGE_ICON_NAME = langentries.getProperty("instance_change_icon_name");

		if (langentries.hasProperty("instance_change_icon_title"))
			INSTANCE_CHANGE_ICON_TITLE = langentries.getProperty("instance_change_icon_title");

		if (langentries.hasProperty("instance_change_icon_unsupported"))
			INSTANCE_CHANGE_ICON_UNSUPPORTED = langentries.getProperty("instance_change_icon_unsupported");

		if (langentries.hasProperty("instance_change_icon_unsupported_title"))
			INSTANCE_CHANGE_ICON_UNSUPPORTED_TITLE = langentries.getProperty("instance_change_icon_unsupported_title");

		if (langentries.hasProperty("instance_change_icon_failed"))
			INSTANCE_CHANGE_ICON_FAILED = langentries.getProperty("instance_change_icon_failed");

		if (langentries.hasProperty("instance_change_icon_failed_title"))
			INSTANCE_CHANGE_ICON_FAILED_TITLE = langentries.getProperty("instance_change_icon_failed_title");

		if (langentries.hasProperty("instance_name"))
			INSTANCE_NAME = langentries.getProperty("instance_name");

		if (langentries.hasProperty("instance_select_addons"))
			INSTANCE_SELECT_ADDONS = langentries.getProperty("instance_select_addons");

		if (langentries.hasProperty("instance_mods_repository"))
			INSTANCE_MODS_REPOSITORY = langentries.getProperty("instance_mods_repository");


		if (langentries.hasProperty("select_instance_title"))
			SELECT_INSTANCE_TITLE = langentries.getProperty("select_instance_title");

		if (langentries.hasProperty("select_instance_new"))
			SELECT_INSTANCE_NEW = langentries.getProperty("select_instance_new");


		if (langentries.hasProperty("new_version_found"))
			UPDATE_FOUND = langentries.getProperty("new_version_found");


		if (langentries.hasProperty("nick"))
			WRAP_USER = langentries.getProperty("nick");

		if (langentries.hasProperty("version"))
			WRAP_VERSION = langentries.getProperty("version");

		if (langentries.hasProperty("server"))
			WRAP_SERVER = langentries.getProperty("server");

		if (langentries.hasProperty("server_title"))
			WRAP_SERVER_TITLE = langentries.getProperty("server_title");

		if (langentries.hasProperty("classic_resize"))
			WRAP_CLASSIC_RESIZE = langentries.getProperty("classic_resize");


		if (langentries.hasProperty("tab_changelog"))
			TAB_CHANGELOG =  langentries.getProperty("tab_changelog");

		if (langentries.hasProperty("tab_instances"))
			TAB_INSTANCES =  langentries.getProperty("tab_instances");

		if (langentries.hasProperty("tab_servers"))
			TAB_SERVERS =  langentries.getProperty("tab_servers");


		if (langentries.hasProperty("version_custom"))
			VERSION_CUSTOM = langentries.getProperty("version_custom");


		if (langentries.hasProperty("browser_title"))
			BROWSER_TITLE = langentries.getProperty("browser_title");


		if (langentries.hasProperty("srv_loading"))
			TAB_SRV_LOADING = langentries.getProperty("srv_loading");

		if (langentries.hasProperty("srv_failed"))
			TAB_SRV_FAILED = langentries.getProperty("srv_failed");

		if (langentries.hasProperty("cl_loading"))
			TAB_CL_LOADING = langentries.getProperty("cl_loading");

		if (langentries.hasProperty("cl_failed"))
			TAB_CL_FAILED = langentries.getProperty("cl_failed");


		if (langentries.hasProperty("force_update"))
			FORCE_UPDATE = langentries.getProperty("force_update");

		if (langentries.hasProperty("console_output_for"))
			CONSOLE_OUTPUT_FOR = langentries.getProperty("console_output_for");

		if (langentries.hasProperty("console_output"))
			CONSOLE_OUTPUT = langentries.getProperty("console_output");


		if (langentries.hasProperty("yes"))
			YES = langentries.getProperty("yes");

		if (langentries.hasProperty("no"))
			NO = langentries.getProperty("no");

		if (langentries.hasProperty("cancel"))
			CANCEL = langentries.getProperty("cancel");

		if (langentries.hasProperty("select"))
			SELECT = langentries.getProperty("select");

		if (langentries.hasProperty("remove"))
			REMOVE = langentries.getProperty("remove");

		if (langentries.hasProperty("browse"))
			BROWSE = langentries.getProperty("browse");

		if (langentries.hasProperty("copy"))
			COPY = langentries.getProperty("copy");

		if (langentries.hasProperty("clear"))
			CLEAR = langentries.getProperty("clear");

		if (langentries.hasProperty("load"))
			LOAD = langentries.getProperty("load");

		if (langentries.hasProperty("close"))
			CLOSE = langentries.getProperty("close");

		if (langentries.hasProperty("link"))
			LINK = langentries.getProperty("link");


		if (langentries.hasProperty("pause"))
			PAUSE = langentries.getProperty("pause");

		if (langentries.hasProperty("unpause"))
			UNPAUSE = langentries.getProperty("unpause");

		applyNamesSwing();

		if (force) {
			Window.mainWindow.update();
			if (Window.addonsList != null) Window.addonsList.update();
			if (Window.modsRepo != null) Window.modsRepo.update();
			if (Window.lang != null) Window.lang.update();
			if (Window.instanceList != null) Window.instanceList.update();
			if (Window.instanceSettings != null) Window.instanceSettings.update();
			if (Window.versionsList != null) Window.versionsList.update();
		}
	}

	public static String UPDATE_FOUND = "There is a new version of the launcher available (%s). Would you like to update?";

	public static String WINDOW_PLAY = "Play";
	public static String WINDOW_SELECT_VERSION = "Select version";
	public static String WINDOW_LANGUAGE = "Language";
	public static String WINDOW_OPTIONS = "Edit instance";
	public static String WINDOW_TITLE = "BetaCraft Launcher v" + Launcher.VERSION;
	public static String WINDOW_DOWNLOADING = "Downloading ...";
	public static String WINDOW_DOWNLOADING_RESOURCE = "Downloading: %s";
	public static String PACKING_MOD = "Packing mod ...";
	public static String MANUAL_DOWNLOAD = "Manual download";
	public static String WINDOW_USERNAME_FIELD_EMPTY = "The username field is empty!";

	public static String LANG = "Select language";

	public static String OPTIONS_PROXY = "Use skin & sound proxy";
	public static String OPTIONS_UPDATE_HEADER = "Update available";
	public static String OPTIONS_KEEP_OPEN = "Keep the launcher open";
	public static String OPTIONS_RPC = "Discord RPC";
	public static String OPTIONS_LAUNCH_ARGS = "JVM arguments:";
	public static String OPTIONS_OK = "OK";
	public static String OPTIONS_WIDTH = "width:";
	public static String OPTIONS_HEIGHT = "height:";
	public static String OPTIONS_TITLE = "Instance settings";

	public static String SORT_FROM_OLDEST = "Sort: from oldest";
	public static String SORT_FROM_NEWEST = "Sort: from newest";
	public static String VERSION_LIST_TITLE = "Select version";

	public static String ADDON_LIST_TITLE = "Addons list";
	public static String ADDON_NO_DESC = "No description.";
	public static String ADDON_SHOW_INFO = "Show info";

	public static String LOGGING_IN = "Logging in...";
	public static String LOGIN_TITLE = "Log in";
	public static String LOGIN_BUTTON = "Log in";
	public static String LOGOUT_BUTTON = "Log out";
	public static String LOGIN_EMAIL_NICKNAME = "E-mail:";
	public static String LOGIN_PASSWORD = "Password:";
	public static String LOGIN_MOJANG_HEADER = "... or login with a Mojang account:";
	public static String LOGIN_MICROSOFT_BUTTON = "Login with Microsoft";
	public static String LOGIN_MICROSOFT_TITLE = "Login with your Microsoft account";
	public static String LOGIN_MICROSOFT_ERROR = "Microsoft authentication error";
	public static String LOGIN_MICROSOFT_PARENT = "Parental approval required. Add this account to Family to login.";
	public static String LOGIN_MICROSOFT_NO_XBOX = "No Xbox account registered";
	public static String LOGIN_MICROSOFT_NO_MINECRAFT = "You don't own Minecraft on this account.";
	public static String LOGIN_MICROSOFT_CODE_LINE1 = "To proceed, open up:";
	public static String LOGIN_MICROSOFT_CODE_LINE2 = "in a browser and type the code:";
	public static String LOGIN_RELOGIN = "You need to re-login to your Microsoft account (token timed out)";

	public static String LOGIN_FAILED = "Failed to complete the login process";
	public static String LOGIN_FAILED_INVALID_CREDENTIALS = "Invalid e-mail or password.";

	public static String JAVA_EXECUTABLE = "Java executable:";
	public static String JAVA_INVALID = "Given Java path is not valid. Do you want the instance to use recommended Java?";
	public static String JAVA_TOO_RECENT = "Given Java version is greater than 8. Do you want the instance to use recommended Java?";
	public static String JAVA_RECOMMENDED_NOT_FOUND = "Could not find recommended Java in your system. Do you want to download and install it?";
	public static String JAVA_WRONG_ARCH = "It seems that you're running the wrong architecture of Java (ARM or 32-bit x86). \nShould you run into issues, get 64-bit Java 8 from https://java.com and make your instance use it.";
	public static String JAVA_SSL_NOT_SUPPORTED = "Your Java is too old to connect to the required resource. Update your Java to %s";
	public static String JAVA_SSL_TO_MICROSOFT_ACCOUNT = "login to your Minecraft account.";
	public static String JAVA_SSL_TO_DOWNLOAD_RESOURCE = "download the required resource.";
	public static String JAVA_INTEL_GRAPHICS = "Your machine has Intel HD Graphics. In order for the game to run as intended, you may have to use Java 8u51 for your instance (your current Java: %s)";
	public static String JAVA_INTEL_GRAPHICS_WIKI = "Learn how to get Java 8u51 here: https://github.com/betacraftuk/betacraft-launcher/wiki/intel";

	public static String INSTANCE_REMOVE_DIRECTORY = "Would you want to permanently remove this instance's directory? This cannot be undone.";
	public static String INSTANCE_DIRECTORY = "Instance directory:";
	public static String INSTANCE_GAME_DIRECTORY_TITLE = "Choose a directory for the instance";
	public static String INSTANCE_REMOVE_QUESTION = "Are you sure you want to remove this instance?";
	public static String INSTANCE_REMOVE_TITLE = "Remove instance";
	public static String INSTANCE_CHANGE_ICON_NAME = "Change icon";
	public static String INSTANCE_CHANGE_ICON_TITLE = "Choose a new icon for the instance";
	public static String INSTANCE_CHANGE_ICON_UNSUPPORTED = "Your icon file format is not supported. Currently supported formats: %s";
	public static String INSTANCE_CHANGE_ICON_UNSUPPORTED_TITLE = "Unsupported image format";
	public static String INSTANCE_CHANGE_ICON_FAILED = "Something went wrong: %s";
	public static String INSTANCE_CHANGE_ICON_FAILED_TITLE = "Failed to save icon";
	public static String INSTANCE_NAME = "Name:";
	public static String INSTANCE_SELECT_ADDONS = "Select addons";
	public static String INSTANCE_MODS_REPOSITORY = "Mods repository";

	public static String SELECT_INSTANCE_TITLE = "Select instance";
	public static String SELECT_INSTANCE_NEW = "New instance";

	public static String TAB_CHANGELOG = "Changelog";
	public static String TAB_INSTANCES = "Instances";
	public static String TAB_SERVERS = "Server list";

	public static String VERSION_CUSTOM = " [Custom]";

	public static String BROWSER_TITLE = "Webpage viewer";

	public static String TAB_SRV_LOADING = "Loading server list...";
	public static String TAB_SRV_FAILED = "Failed to list servers!";
	public static String TAB_CL_LOADING = "Loading update news...";
	public static String TAB_CL_FAILED = "Failed to load update news!";

	public static String FORCE_UPDATE = "Force update";
	public static String CONSOLE_OUTPUT_FOR = "Game log for \"%s\"";
	public static String CONSOLE_OUTPUT = "Game log";

	public static String WRAP_USER = "User: %s";
	public static String WRAP_VERSION = "Version: %s";
	public static String WRAP_SERVER = "Server IP (leave blank if you don't want to play online):";
	public static String WRAP_SERVER_TITLE = "Server IP";
	public static String WRAP_CLASSIC_RESIZE = "<html><font size=5>Resize the window to the size you want to play on.<br />Click anywhere inside this window to start the game.</font></html>";

	public static String UNEXPECTED_ERROR = "Unexpected error: %s";
	public static String ERR_NO_CONNECTION = "No stable internet connection.";
	public static String ERR_DL_FAIL = "Download failed.";

	public static String YES = "Yes";
	public static String NO = "No";
	public static String CANCEL = "Cancel";
	public static String SELECT = "Select";
	public static String REMOVE = "Remove";
	public static String BROWSE = "Browse";
	public static String COPY = "Copy";
	public static String CLEAR = "Clear";
	public static String LOAD = "Load";
	public static String CLOSE = "Close";
	public static String LINK = "Link";

	public static String PAUSE = "Pause";
	public static String UNPAUSE = "Unpause";
}
