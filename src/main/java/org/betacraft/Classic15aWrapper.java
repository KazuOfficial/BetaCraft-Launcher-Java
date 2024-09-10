package org.betacraft;

import java.awt.BorderLayout;
import java.awt.Canvas;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Image;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.ArrayList;

import javax.swing.JLabel;
import javax.swing.JPanel;

import org.betacraft.launcher.Lang;

import net.arikia.dev.drpc.DiscordRPC;

// Pretends to be MinecraftApplet
public class Classic15aWrapper extends Wrapper {
	public Runnable run;
	public Thread thread;

	public Classic15aWrapper(String user, String ver_prefix, String version, String sessionid, String mainFolder,
			Integer height, Integer width, Boolean RPC, String launchMethod, String server, String mppass, String uuid,
			String USR, String VER, Image img, ArrayList addons) {
		super(user, ver_prefix, version, sessionid, mainFolder, height, width, RPC, launchMethod, server, mppass, uuid, USR, VER, img,
				addons);
	}

	@Override
	public void play() {
		this.defaultPort = "5565";
		this.askForServer();
		try {
			this.loadJars();

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
			panel.setPreferredSize(new Dimension(width, height));

			if (this.resize_applet) {

				JLabel infolabel1 = new JLabel(Lang.WRAP_CLASSIC_RESIZE);
				infolabel1.setBackground(Color.BLACK);
				infolabel1.setForeground(Color.WHITE);
				panel.add(infolabel1, BorderLayout.CENTER);

				panel.addMouseListener(new MouseListener() {

					public void mouseClicked(MouseEvent e) {
						gameFrame.removeAll();
						gameFrame.setLayout(new BorderLayout());
						gameFrame.add(Classic15aWrapper.this, "Center");
						Classic15aWrapper.this.init();
						active = true;
						Classic15aWrapper.this.start();

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

			// Add game's applet to this window
			this.setLayout(new BorderLayout());

			if (!this.resize_applet) {
				gameFrame.removeAll();
				gameFrame.setLayout(new BorderLayout());
				gameFrame.add(Classic15aWrapper.this, "Center");
				Classic15aWrapper.this.init();
				active = true;
				Classic15aWrapper.this.start();

				gameFrame.validate();

				// Start Discord RPC
				if (discord) discordThread.start();
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	@Override
	public void start() {}

	@Override
	public void init() {
		try {
			if (!this.addonsPreAppletInit(this.addons)) return;
			Canvas canvas = new Canvas();
			for (final Field minecraftField : mainClass.getDeclaredFields()) {
				String name = minecraftField.getType().getName();
				if (name.contains("mojang")) {
					final Class<?> clazz = classLoader.loadClass(name);
					Constructor<?> con = clazz.getConstructor(java.awt.Canvas.class, int.class, int.class, boolean.class);
					run = (Runnable) con.newInstance(params.containsKey("fullscreen") ? null : canvas, panel.getWidth(), 
							panel.getHeight(), params.containsKey("fullscreen"));

					mainClassInstance = run;
					setResolution(run, canvas, clazz);
				}
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	@Override
	public void stop() {
		if (!active) {
			return;
		}
		// Shutdown the RPC correctly
		if (discord) DiscordRPC.discordShutdown();
		active = false;
		try {
			for (final Field mcField : mainClass.getDeclaredFields()) {
				String name = mcField.getType().getName();
				if (name.contains("mojang")) {
					final Class<?> clazz = classLoader.loadClass(name);
					mcField.setAccessible(true);
					for (Field pauseField : clazz.getDeclaredFields()) {
						int mod = pauseField.getModifiers();
						if (Modifier.isVolatile(mod) && Modifier.isPublic(mod) && pauseField.getType().getName().equals("boolean")) {
							pauseField.set(run, true);
						}
					}
					for (Field runningField : clazz.getDeclaredFields()) {
						int mod = runningField.getModifiers();
						if (Modifier.isVolatile(mod) && !Modifier.isPublic(mod) && runningField.getType().getName().equals("boolean")) {
							runningField.setAccessible(true);
							runningField.set(run, false);
						}
					}
					clazz.getDeclaredMethod("b").invoke(run);
				}
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	@Override
	public void destroy() {
		if (!active) {
			return;
		}
		this.stop();
		try {
			this.thread.join(5000L);
		} catch (InterruptedException e) {
			try {
				for (final Field mcField : mainClass.getDeclaredFields()) {
					String name = mcField.getType().getName();
					if (name.contains("mojang")) {
						final Class<?> clazz = classLoader.loadClass(name);
						mcField.setAccessible(true);
						clazz.getDeclaredMethod("a").invoke(run);
					}
				}
			}
			catch (Exception ee) {
				ee.printStackTrace();
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	public void setResolution(Runnable run, Canvas canvas, Class<?> clazz) {
		try {

			Field resource = clazz.getDeclaredField("f");
			resource.setAccessible(true);
			resource.set(run, this.getDocumentBase().getHost() + ":" + this.getDocumentBase().getPort());
			if (this.getParameter("username") != null && this.getParameter("sessionid") != null) {
				Field credentials = clazz.getDeclaredField("e");
				credentials.setAccessible(true);
				Object credinstance = this.classLoader.loadClass("com.mojang.minecraft.a").getConstructor(String.class, String.class).newInstance(this.getParameter("username"), this.getParameter("sessionid"));

				credentials.set(run, credinstance);
			}
			if (this.getParameter("server") != null) {
				int port = Integer.parseInt(this.getParameter("port"));
				try {
					Class c = this.classLoader.loadClass("com.mojang.minecraft.net.b");
					Constructor constr = c.getConstructor(clazz, String.class, int.class, String.class);
					constr.setAccessible(true);
					Object Cc = constr.newInstance(run, 
							this.getParameter("server"), port, 
							this.getParameter("username"));

					Field field = clazz.getDeclaredField("C");
					field.setAccessible(true);
					field.set(run, Cc);
				} catch (Exception ex) {
					ex.printStackTrace();
				}
				// previous method, but this has hardcoded port
				//clazz.getDeclaredMethod("a", String.class, int.class).invoke(run, this.getParameter("server"), port);
			}
			if (this.getParameter("loadmap_user") != null && this.getParameter("loadmap_id") != null) {
				Field mapuser = clazz.getDeclaredField("j");
				mapuser.setAccessible(true);
				mapuser.set(run, this.getParameter("loadmap_user"));
				Field mapid = clazz.getDeclaredField("k");
				mapid.setAccessible(true);
				mapid.set(run, Integer.parseInt(this.getParameter("loadmap_id")));
			}
			for (final Field appletModeField : clazz.getDeclaredFields()) {
				if (appletModeField.getType().getName().equalsIgnoreCase("boolean") && Modifier.isPublic(appletModeField.getModifiers())) {
					appletModeField.setAccessible(true);
					appletModeField.set(run, params.containsKey("offlinesaving") ? false : !params.containsKey("fullscreen"));
					break;
				}
			}
			for (final Field fullscreenField : clazz.getDeclaredFields()) {
				if (fullscreenField.getType().getName().equalsIgnoreCase("boolean") && Modifier.isPrivate(fullscreenField.getModifiers())) {
					fullscreenField.setAccessible(true);
					fullscreenField.set(run, params.containsKey("fullscreen"));
					break;
				}
			}
			this.setLayout(new BorderLayout());
			this.add(canvas, "Center");
			(thread = new Thread(run)).start();
			canvas.setFocusable(true);
			this.validate();
			addonsPostAppletInit(this.addons);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}
}
