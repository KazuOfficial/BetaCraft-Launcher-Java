package org.betacraft.launcher;

import java.io.File;

import javax.swing.JOptionPane;

import org.betacraft.launcher.Util.PropertyFile;

public class BC {
	public static File currentPath;
	public static File SETTINGS_FILE;
	public static PropertyFile SETTINGS;

	// TODO better check this before release
	public static boolean prerelease = false;
	public static boolean nightly = false;

	public static boolean portable = false;
	public static boolean wrapped = false;

	public static String get() {
		if (OS.isWindows()) {
			return windowsPath();
		} else {
			return path();
		}
	}

	public static String windowsPath() {
		if (portable) return prefBC() + "\\.betacraft\\";
		return System.getenv("APPDATA") + "\\.betacraft\\";
	}

	public static String path() {
		String folder = null;
		if (OS.isLinux()) {
			folder = System.getProperty("user.home") + "/.betacraft/";
			if (portable) folder = prefBC() + "/.betacraft/";
		} else if (OS.isMac()) {
			folder = System.getProperty("user.home") + "/Library/Application Support/betacraft/";
			if (portable) folder = prefBC() + "/betacraft/";
		} else {
			System.out.println("Your operating system is not supported.");
			JOptionPane.showMessageDialog(Window.mainWindow, "Your operating system is not supported ;(", "I'm sorry, but", JOptionPane.WARNING_MESSAGE);
			folder = prefBC() + "/betacraft/";
		}

		File betacraft = new File(folder);
		betacraft.mkdirs();
		return folder;
	}

	public static String prefBC() {
		if (wrapped) return currentPath.getAbsoluteFile().getParentFile().getParentFile().getParent();
		return currentPath.getAbsoluteFile().getParent();
	}

	public static String trimBetaCraftDir(String path) {
		return path.substring(BC.get().length());
	}
}
