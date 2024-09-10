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
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.ArrayList;

import javax.swing.JLabel;
import javax.swing.JPanel;

import org.betacraft.launcher.Lang;
import org.betacraft.launcher.OS;

import net.arikia.dev.drpc.DiscordRPC;

// Pretends to be MinecraftApplet
public class Classic12aWrapper extends Wrapper {
	public Runnable run;
	public Thread thread;

	public Classic12aWrapper(String user, String ver_prefix, String version, String sessionid, String mainFolder,
			Integer height, Integer width, Boolean RPC, String launchMethod, String server, String mppass, String uuid, String USR,
			String VER, Image img, ArrayList addons) {
		super(user, ver_prefix, version, sessionid, mainFolder, height, width, RPC, launchMethod, server, mppass, uuid, USR, VER, img,
				addons);
	}

	@Override
	public void play() {
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
						gameFrame.add(Classic12aWrapper.this, "Center");
						Classic12aWrapper.this.init();
						active = true;
						Classic12aWrapper.this.start();

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
				gameFrame.add(Classic12aWrapper.this, "Center");
				Classic12aWrapper.this.init();
				active = true;
				Classic12aWrapper.this.start();

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
					run = (Runnable) con.newInstance(params.containsKey("fullscreen") ? null : canvas, panel.getWidth(), panel.getHeight(), params.containsKey("fullscreen"));
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

					Method real12a;
					try {
						real12a = clazz.getDeclaredMethod("a");
					} catch (NoSuchMethodException noSuchMethodException) {
						// 12a-dev
						real12a = clazz.getDeclaredMethod("stop");
					}

					real12a.invoke(run);
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

						Method real12a;
						try {
							real12a = clazz.getDeclaredMethod("a");
						} catch (NoSuchMethodException noSuchMethodException) {
							// 12a-dev
							real12a = clazz.getDeclaredMethod("destroy");
						}

						real12a.invoke(run);
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
			for (Field widthHeightField : clazz.getDeclaredFields()) {
				if (widthHeightField.getName().equals("a")) {
					widthHeightField.set(run, panel.getWidth());
				}
				if (widthHeightField.getName().equals("b")) {
					widthHeightField.set(run, panel.getHeight());
				}
			}
			for (final Field appletModeField : clazz.getDeclaredFields()) {
				if (appletModeField.getType().getName().equalsIgnoreCase("boolean") && Modifier.isPublic(appletModeField.getModifiers())) {
					appletModeField.setAccessible(true);
					appletModeField.set(run, OS.isLinux() || params.containsKey("fullscreen") ? false : true);
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
