package org.betacraft;

import java.applet.Applet;
import java.applet.AppletStub;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.FilenameFilter;
import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

import org.betacraft.Addon.WhatToDo;
import org.betacraft.launcher.BC;
import org.betacraft.launcher.Lang;

import com.johnymuffin.evolutions.core.BetaEvolutionsUtils;
import com.johnymuffin.evolutions.core.BetaEvolutionsUtils.VerificationResults;

import net.arikia.dev.drpc.DiscordEventHandlers;
import net.arikia.dev.drpc.DiscordRPC;
import net.arikia.dev.drpc.DiscordRichPresence;
import uk.betacraft.auth.jsons.mojang.session.JoinServerRequest;
import uk.betacraft.util.WebData;


public class Wrapper extends Applet implements AppletStub {

	/** Client parameters */
	public final Map<String, String> params = new HashMap<String, String>();
	/** Session id for server authentication */
	public String session;
	/** User ID required to get mppass */
	public String uuid;
	/** Instance's folder */
	public String mainFolder;
	/** Version to be launched */
	public String version;
	/** Class loader for linking the game and libraries */
	public URLClassLoader classLoader;
	/** Minecraft's main class */
	public Class mainClass;
	/** Discord RPC */
	public boolean discord = false;
	/** Icon for the window frame */
	public Image icon;

	public Frame gameFrame;
	public JPanel panel;
	/** Applet of the game */
	public Object mainClassInstance = null;
	public int context = 0;
	public boolean active = false;
	/** Name for the window frame */
	public String window_name = "";
	public DiscordThread discordThread = null;

	/** Preferred width of the game applet */
	public int width = 854;
	/** Preferred height of the game applet */
	public int height = 480;
	public boolean resize_applet = false;
	public boolean ask_for_server = false;

	public int portCompat = 80;
	public String serverAddress = null;
	public String mppass = "0";
	public String defaultPort = "25565";

	/** List of addons to be applied to this instance */
	public ArrayList<Addon> addons = new ArrayList<Addon>();
	public ArrayList<Class<Addon>> ogaddons = new ArrayList<Class<Addon>>();

	/** Tells whether lwjgl dependencies have been already loaded or not */
	public boolean libraries_loaded = false;

	/**
	 * Initializes the Wrapper class & enables the game
	 * 
	 * @param user - Username
	 * @param ver_prefix - Name for the window frame
	 * @param version - Version of the game to play
	 * @param sessionid - Session id of the account (token:accessToken:profileid)
	 * @param mainFolder - Folder of the instance
	 * @param height - Preferred height of the applet
	 * @param width - Preferred width of the applet
	 * @param RPC - Discord Rich Presence
	 * @param launchMethod - Launch method for the version
	 * @param server - Server parameters
	 * @param mppass - Authentication string for Classic servers
	 * @param USR - Discord RPC username string
	 * @param VER - Discord RPC version string
	 * @param img - Icon for the window frame
	 * @param addons - List of addons to apply to this instance
	 */
	public Wrapper(String user, String ver_prefix, String version, String sessionid, String mainFolder, Integer height, Integer width, Boolean RPC, String launchMethod, String server, String mppass, String uuid, String USR, String VER, Image img, ArrayList addons) {
		ogaddons = (ArrayList<Class<Addon>>)addons;

		params.put("username", user);
		params.put("sessionid", sessionid);
		params.put("haspaid", "true");

		if (uuid != null) {
			params.put("uuid", uuid);
		}

		if (server != null && server.contains(":")) {
			params.put("server", server.split(":")[0]);
			params.put("port", server.split(":")[1]);
		}

		this.version = version;
		this.session = sessionid;
		this.uuid = uuid;
		this.launchType = launchMethod;
		if (this.launchType.equalsIgnoreCase("")) {
			System.err.println("LAUNCH METHOD ISN'T SPECIFIED!!! CANNOT PROCEED! CLOSING!");
			System.exit(0);
		}
		this.mainFolder = mainFolder;
		this.height = height;
		this.width = width;
		this.discord = RPC;
		this.serverAddress = server;
		this.mppass = mppass;
		this.icon = img;
		this.window_name = ver_prefix;

		new File(this.mainFolder).mkdirs();

		String port = System.getProperty("http.proxyPort");

		if (port != null) {
			this.portCompat = Integer.parseInt(port);
		}

		try {
			this.libraries_loaded = Boolean.parseBoolean(System.getProperty("betacraft.loaded_libraries"));
		} catch (Throwable t) {}

		try {
			this.resize_applet = Boolean.parseBoolean(System.getProperty("betacraft.resize_applet"));
		}  catch (Throwable t) {}

		try {
			this.ask_for_server = Boolean.parseBoolean(System.getProperty("betacraft.ask_for_server"));
		}  catch (Throwable t) {}

		if (this.discord) {
			String applicationId = "939918927989973052";
			DiscordEventHandlers handlers = new DiscordEventHandlers();
			DiscordRPC.discordInitialize(applicationId, handlers, true);

			DiscordRichPresence presence = new DiscordRichPresence();
			presence.startTimestamp = System.currentTimeMillis() / 1000;
			presence.state = String.format(VER, version);
			presence.details = String.format(USR, user);
			presence.largeImageKey = "logo_betacraft_1024";
			presence.largeImageText = "Download at betacraft.uk";
			DiscordRPC.discordUpdatePresence(presence);
			discordThread = new DiscordThread();
		}

		play();
	}

	/**
	 * Loads addons in the correct order
	 * 
	 * @param a - Addon to load
	 */
	public void loadAddon(Addon a) {
		if (addonLoaded(a)) return;
		List<String> applyAfter = a.applyAfter();
		for (String addon : applyAfter) {
			for (Class<Addon> a1 : ogaddons) {
				if (a1.getName().contains(addon)) {
					try {
						loadAddon((Addon) a1.newInstance());
					} catch (Exception e) {
						e.printStackTrace();
					}
				}
			}
		}
		this.addons.add(a);
	}

	public boolean addonLoaded(Addon a) {
		for (Addon addon : this.addons) {
			if (addon.getName().equals(a.getName())) return true;
		}
		return false;
	}

	public class DiscordThread extends Thread {

		DiscordThread() {
			super("RPC-Callback-Handler");
		}

		// Update the RPC
		public void run() {
			while (active) {
				DiscordRPC.discordRunCallbacks();
				try {
					Thread.sleep(2000);
				} catch (InterruptedException ignored) {}
			}
		}
	}
	
	public void sendJoinServerRequest(String server) {
		System.out.println("Sending joinServer request...");
		
		InetAddress addr;
		try {
			addr = InetAddress.getByName(server);
			
			if (!addr.isAnyLocalAddress() && !addr.isLoopbackAddress())
				server = new BetaEvolutionsUtils().getExternalIP();
			else if (server.equals("localhost"))
				server = "127.0.0.1";
			
		} catch (UnknownHostException e) {
			e.printStackTrace();
		}

		WebData data = new JoinServerRequest(this.session, this.uuid, server).perform().getData();

		if (data.getResponseCode() == -2) {
			JOptionPane.showMessageDialog(null, String.format(Lang.JAVA_SSL_NOT_SUPPORTED, Lang.JAVA_SSL_TO_AUTHENTICATE), "", JOptionPane.ERROR_MESSAGE);
		}
		
		if (data.successful()) {
			System.out.println("Done!");
		} else {
			System.out.println("Failed");
		}
	}

	@Deprecated
	public void getMPpass(String server) {
		this.mppass = "0";

		sendJoinServerRequest(server.split(":")[0]);
	}

	/**
	 * Asks the user for server credentials.
	 */
	public void askForServer() {
		if (this.serverAddress != null) {
			this.sendJoinServerRequest(this.serverAddress.split(":")[0]);

			params.put("mppass", this.mppass);
		} else {
			String server = JOptionPane.showInputDialog(this, Lang.WRAP_SERVER, Lang.WRAP_SERVER_TITLE, JOptionPane.DEFAULT_OPTION);
			String port = this.defaultPort;
			if (server != null && !server.equals("")) {
				String IP = server;
				if (server.startsWith("mc://")) {
					// mc://<ip>:<port>/<username>/<mppass>
					String[] splitted = server.split("/");
					String[] hostport = splitted[2].split(":");
					IP = hostport[0];
					port = hostport[1];
					this.mppass = splitted[4];
				} else if (server.startsWith("join://")) {
					// join://<ip>:<port>/<mppass>/<protocol>/<prefferedversion>
					String[] splitted = server.split("/");
					String[] hostport = splitted[2].split(":");
					IP = hostport[0];
					port = hostport[1];
					this.mppass = splitted[3];
				} else if (IP.contains(":")) {
					// <ip>:<port>
					String[] params1 = server.split(":");
					IP = params1[0];
					port = params1[1];
				}

				this.sendJoinServerRequest(IP);

				// <ip>

				System.out.println("Accepted server parameters: " + IP + ":" + port + this.mppass != null ? " + mppass" : "");
				params.put("server", IP);
				params.put("port", port);
				params.put("mppass", this.mppass);
			}
		}
	}

	/**
	 * Authenticates with Beta Evolutions
	 */
	public void authenticateBetaEvo() {
		BetaEvolutionsUtils be = new BetaEvolutionsUtils(false);
		VerificationResults res = be.authenticateUser(params.get("username"), this.session);
		System.out.println("[Beta Evolutions] Authenticated with " + res.getSuccessful() + " nodes");
	}

	public String launchType = null;

	/**
	 * Replaced with {@link #loadMainClass(URL[])}
	 * 
	 * @param url - Array of jar links for the class loader
	 */
	@Deprecated
	public void setPrefixAndLoadMainClass(URL[] url) {
		this.loadMainClass(url);
	}

	/**
	 * Loads the main game class and initializes it.
	 * Additionally it loads the appropriate non-custom launch method if launch type is empty.
	 * 
	 * @param url - Array of jar links for the class loader
	 */
	public void loadMainClass(URL[] url) {

		try {
			classLoader = null;
			URL[] old = url.clone();
			URL[] neww = new URL[old.length];
			int i;
			for (i = 0; i < old.length; i++) {
				neww[i] = old[i];
			}
			classLoader = new URLClassLoader(neww);
			try {
				System.out.println();
				System.out.println("Loading addons:");
				for (Class<Addon> c : ogaddons) {
					this.loadAddon((Addon) c.newInstance());
					System.out.println("- " + c);
				}
				System.out.println();
			} catch (Exception ex) {
				ex.printStackTrace();
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		}

		// I know this looks terrible, but it works!
		try {
			// Classic
			mainClass = classLoader.loadClass("com.mojang.minecraft.MinecraftApplet");
		} catch (ClassNotFoundException ex) {
			try {
				mainClass = classLoader.loadClass("com.mojang.rubydung.RubyDung");

			} catch (ClassNotFoundException ex2) {
				try {
					mainClass = classLoader.loadClass("com.mojang.minecraft.RubyDung");

				} catch (ClassNotFoundException ex3) {
					try {
						// Indev+
						mainClass = classLoader.loadClass("net.minecraft.client.MinecraftApplet");
					} catch (ClassNotFoundException ex4) {
						try {
							mainClass = classLoader.loadClass("M");
						} catch (ClassNotFoundException ex5) {}
					}
				}// catch (Exception ex3) {}
			}// catch (Exception ex2) {}
		}

		try {
			mainClassInstance = mainClass.newInstance();
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	/**
	 * Sets the game folder to {@link #mainFolder}
	 */
	public void setGameFolder() {
		try {
			if (launchType.equals("indev")) {
				for (final Field field : mainClass.getDeclaredFields()) {
					final String name = field.getType().getName();
					if (!name.contains("awt") && !name.contains("java") && !name.contains("long")) {
						Field fileField = null;
						final Class clazz = classLoader.loadClass(name);
						for (final Field field1 : clazz.getDeclaredFields()) {
							if (Modifier.isStatic(field1.getModifiers()) && field1.getType().getName().equals("java.io.File")) {
								fileField = field1;
							}
						}
						if (fileField != null) {
							fileField.setAccessible(true);
							fileField.set(null, new File(this.mainFolder));
							break;
						}
					}
				}
			}
		} catch (ClassNotFoundException ex) {
			String err = "Error code 6 (MISSING): Couldn't satisfy the wrapper with a valid .jar file. Check your version JAR or launch configuration file.";
			System.err.println(err);
			JOptionPane.showMessageDialog(gameFrame, err, "Error", JOptionPane.INFORMATION_MESSAGE);
			ex.printStackTrace();
			System.exit(0);
		} catch (NoClassDefFoundError ex) {
			String err = "Error code 5 (LIB_MISSING): Some of the required libraries are missing. Select \"Force update\" in instance settings to re-download them.";
			System.err.println(err);
			JOptionPane.showMessageDialog(gameFrame, err, "Error", JOptionPane.INFORMATION_MESSAGE);
			ex.printStackTrace();
			System.exit(0);
		} catch (Throwable ex) {
			ex.printStackTrace();
		}
	}

	public void loadJars() {
		try {
			String[] libs = new File(BC.get(), "bin/").list(new FilenameFilter() {
				public boolean accept(File dir, String fileName) {
					return fileName.endsWith(".jar");
				}
			});

			// Glue everything Minecraft needs for running
			String[] files = new String[1 + (this.libraries_loaded ? 0 : libs.length)];

			files[0] = BC.get() + "versions/" + version + ".jar";

			if (!this.libraries_loaded) {
				for (int i = 0; i < libs.length; i++) {
					files[i + 1] = BC.get() + "bin/" + libs[i];
				}
			}

			String nativesPath = BC.get() + "bin/natives";
			System.setProperty("org.lwjgl.librarypath", nativesPath);
			System.setProperty("net.java.games.input.librarypath", nativesPath);

			System.out.println();
			System.out.println("Classpath:");
			final URL[] url = new URL[files.length];
			for (int i = 0; i < files.length; i++) {
				System.out.println(files[i]);
				url[i] = new File(files[i]).toURI().toURL();
			}
			System.out.println();

			loadMainClass(url);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	public void play() {
		this.authenticateBetaEvo();
		if (this.ask_for_server) this.askForServer();
		try {
			this.loadJars();

			// Replace the main game folder to .betacraft
			// Skip versions prior to Indev. They don't support changing game folders.
			this.setGameFolder();

			// Make a frame for the game
			gameFrame = new Frame();
			gameFrame.setTitle(window_name);
			gameFrame.setIconImage(this.icon);
			gameFrame.setBackground(Color.BLACK);
			this.addHooks();

			// This is needed for the window size
			panel = new JPanel();
			panel.setLayout(new BorderLayout());
			gameFrame.setLayout(new BorderLayout());
			panel.setBackground(Color.BLACK);
			panel.setPreferredSize(new Dimension(width, height)); // 854, 480

			final Applet a = (Applet) mainClassInstance;

			if (this.resize_applet) {

				JLabel infolabel1 = new JLabel(Lang.WRAP_CLASSIC_RESIZE);
				infolabel1.setBackground(Color.BLACK);
				infolabel1.setForeground(Color.WHITE);
				panel.add(infolabel1, BorderLayout.CENTER);

				panel.addMouseListener(new MouseListener() {

					public void mouseClicked(MouseEvent e) {
						width = panel.getWidth();
						height = panel.getHeight();
						a.resize(width, height);
						a.setSize(new Dimension(width, height));
						gameFrame.removeAll();
						gameFrame.setLayout(new BorderLayout());
						gameFrame.add(Wrapper.this, "Center");
						Wrapper.this.init();
						active = true;
						Wrapper.this.start();

						gameFrame.validate();

						// Start Discord RPC
						if (discord) discordThread.start();
					}

					public void mouseEntered(MouseEvent arg0) {}
					public void mouseExited(MouseEvent arg0) {}
					public void mousePressed(MouseEvent arg0) {}
					public void mouseReleased(MouseEvent arg0) {}

				});
			}

			gameFrame.add(panel, "Center");
			gameFrame.pack();
			gameFrame.setLocationRelativeTo(null);
			gameFrame.setVisible(true);

			a.setLayout(new BorderLayout());
			a.setStub(this);

			// Add game's applet to this window
			this.setLayout(new BorderLayout());
			this.add(a, "Center");

			if (!this.resize_applet) {
				a.resize(width, height);
				a.setSize(new Dimension(width, height));

				gameFrame.removeAll();
				gameFrame.setLayout(new BorderLayout());
				gameFrame.add(Wrapper.this, "Center");
				Wrapper.this.init();
				active = true;
				Wrapper.this.start();

				gameFrame.validate();

				// Start Discord RPC
				if (discord) discordThread.start();
			}
		} catch (Throwable ex) {
			System.err.println("A critical error has occurred!");
			ex.printStackTrace();
		}
	}

	public void addHooks() {
		gameFrame.addWindowListener(new WindowAdapter() {
			@Override
			public void windowClosing(final WindowEvent e) {
				//stop();
				destroy();
				gameFrame.setVisible(false);
				gameFrame.dispose();
				System.exit(0);
			}
		});
	}

	public void appletResize(int width, int height) {}

	@Override
	public void update(final Graphics g) {
		this.paint(g);
	}

	@Override
	public boolean isActive() {
		if (context == 0) {
			context = -1;
			try {
				if (this.getAppletContext() != null) {
					context = 1;
				}
			} catch (Exception ex) {}
		}
		if (context == -1) {
			return active;
		}
		return super.isActive();
	}

	@Override
	public void stop() {
		if (!active) {
			return;
		}
		// Shutdown the RPC correctly
		if (discord) DiscordRPC.discordShutdown();
		active = false;
		if (mainClassInstance != null) {
			try {
				if (mainClassInstance instanceof Applet) {
					((Applet) mainClassInstance).stop();
				}
			} catch (Exception ex) {
				ex.printStackTrace();
				System.exit(0);
			}
		}
	}

	@Override
	public void destroy() {
		if (!active) {
			return;
		}
		if (mainClassInstance != null) {
			try {
				if (mainClassInstance instanceof Applet) {
					((Applet) mainClassInstance).destroy();
				}
			} catch (Exception ex) {
				ex.printStackTrace();
				System.exit(0);
			}
		}
	}

	@Override
	public void start() {
		if (mainClassInstance != null) {
			try {
				if (mainClassInstance instanceof Applet) {
					((Applet) mainClassInstance).start();
				}
			} catch (Exception ex) {
				ex.printStackTrace();
				System.exit(0);
			}
		}
	}

	@Override
	public void init() {
		if (mainClassInstance != null) {
			if (!this.addonsPreAppletInit(this.addons)) return;
			//System.setProperty("user.home", mainFolder);
			((Applet)mainClassInstance).init();

			// Linux mouse fix, really ugly
			if ("true".equalsIgnoreCase(System.getProperty("betacraft.linux_mousefix_earlyclassic"))) {
				try {
					for (final Field mcField : mainClass.getDeclaredFields()) {
						String name = mcField.getType().getName();
						if (name.contains("minecraft")) {
							final Class<?> clazz = classLoader.loadClass(name);
							mcField.setAccessible(true);
							Object mc = mcField.get(mainClassInstance);

							for (final Field appletModeField : clazz.getDeclaredFields()) {
								if (appletModeField.getType().getName().equalsIgnoreCase("boolean") && Modifier.isPublic(appletModeField.getModifiers())) {
									appletModeField.setAccessible(true);
									appletModeField.set(mc, false);
									System.out.println("Linux mouse fix for early classic has been applied");
									break;
								}
							}
							break;
						}
					}
				} catch (Throwable t) {
					t.printStackTrace();
				}
			}

			if (!this.addonsPostAppletInit(this.addons)) return;
		}
	}

	public boolean addonsPreAppletInit(ArrayList<Addon> toinit) {
		try {
			boolean stop = false;
			ArrayList<Addon> left = (ArrayList<Addon>) toinit.clone();
			for (Addon a : toinit) {
				left.remove(a);
				WhatToDo todo = a.preAppletInit(this, left);
				if (todo == WhatToDo.STOP_LOOP) break;
				if (todo == WhatToDo.STOP_CODE) stop = true;
			}
			return !stop;
		} catch (Exception ex) {
			ex.printStackTrace();
			return true;
		}
	}

	public boolean addonsPostAppletInit(ArrayList<Addon> toinit) {
		try {
			boolean stop = false;
			ArrayList<Addon> left = (ArrayList<Addon>) toinit.clone();
			for (Addon a : toinit) {
				left.remove(a);
				WhatToDo todo = a.postAppletInit(this, left);
				if (todo == WhatToDo.STOP_LOOP) break;
				if (todo == WhatToDo.STOP_CODE) stop = true;
			}
			return !stop;
		} catch (Exception ex) {
			ex.printStackTrace();
			return true;
		}
	}

	@Override
	public URL getDocumentBase() {
		try {
			URL url = new URL("http://www.minecraft.net/game/");
			if (this.mainClass != null) {
				if (mainClassInstance.getClass().getCanonicalName().startsWith("com.mojang")) {
					url = new URL("http", "www.minecraft.net", portCompat, "/game/", null);
				}
			}
			return url;
		}
		catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}

	@Override
	public URL getCodeBase() {
		try {
			return new URL("http://www.minecraft.net/game/");
		}
		catch (MalformedURLException e) {
			e.printStackTrace();
			return null;
		}
	}

	@Override
	public String getParameter(final String paramName) {
		System.out.println("Client asked for parameter: " + paramName);
		if (params.containsKey(paramName)) {
			return params.get(paramName);
		}
		return null;
	}
}
