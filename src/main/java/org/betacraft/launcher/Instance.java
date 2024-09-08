package org.betacraft.launcher;

import java.awt.Image;
import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.imageio.ImageIO;

import org.betacraft.launcher.Util.PropertyFile;

public class Instance {

	public final String name;
	public String version;
	public String launchArgs;
	public int width;
	public int height;
	public boolean proxy;
	public boolean keepopen;
	public boolean RPC;
	public boolean console;
	public List<String> addons;
	public String gameDir;
	public String javaPath;
	public boolean intelFix;
	public boolean isJavaPathNew;

	private Instance(String name, String launchArgs, String version, int width, int height, boolean proxy, boolean keepopen, boolean RPC, List<String> addons, String gameDir) {
		this.name = name;
		this.launchArgs = launchArgs;
		this.width = width;
		this.height = height;
		this.proxy = proxy;
		this.keepopen = keepopen;
		this.RPC = RPC;
		this.addons = addons;
		this.version = version;
		this.gameDir = gameDir;
		this.console = false;
		this.intelFix = true;
		this.javaPath = Launcher.javaRuntime.getAbsolutePath();
		this.isJavaPathNew = true;
	}

	public static Instance newInstance(String name) {
		// Use default settings
		return new Instance(name, "-Xmx1G", "b1.7.3", 854, 480, true, false, true, new ArrayList<String>(), BC.get() + name + "/");
	}

	public static Instance loadInstance(String name) {
		try {
			File instanceFile = new File(BC.get() + "launcher" + File.separator + "instances", name + ".txt");
			if (!instanceFile.exists()) {
				System.out.println(instanceFile.getAbsolutePath());
				System.err.println("Instance file is null!");
				return null;
			}
			PropertyFile instancesettings = new PropertyFile(instanceFile);
			Instance instance = newInstance(name);
			try {
				instance.launchArgs = instancesettings.getProperty("launchArgs");
				String addonz1 = instancesettings.getProperty("addons");
				String[] addonz = addonz1.split(",");
				if (!addonz1.equals("")) {
					for (String addon : addonz) {
						if (instance.addons.contains(addon)) continue;
						instance.addons.add(addon);
					}
				}
				instance.gameDir = instancesettings.getProperty("gameDir");
				instance.version = instancesettings.getProperty("version");

				String width = instancesettings.getProperty("width");
				String height = instancesettings.getProperty("height");

				try {
					instance.width = Integer.parseInt(width);
					instance.height = Integer.parseInt(height);
				} catch (NumberFormatException exx) {
					System.err.println("Failed to parse width and height parameters in instance: " + name);
					return null;
				}

				instance.proxy = Boolean.parseBoolean(instancesettings.getProperty("proxy"));
				instance.keepopen = Boolean.parseBoolean(instancesettings.getProperty("keepopen"));
				instance.RPC = Boolean.parseBoolean(instancesettings.getProperty("RPC"));
				instance.console = Boolean.parseBoolean(instancesettings.getProperty("console"));
				String intelFix = instancesettings.getProperty("intelFix");
				if (intelFix != null) {
					instance.intelFix = Boolean.parseBoolean(intelFix);
				} else {
					instance.intelFix = true;
				}

				String jpath = instancesettings.getProperty("javaPath");
				if (jpath == null || "".equals(jpath)) {
					instance.javaPath = Launcher.javaRuntime.getAbsolutePath();
				} else {
					instance.javaPath = jpath;
				}

				try {
					instance.isJavaPathNew = Boolean.parseBoolean(instancesettings.getProperty("isJavaPathNew"));
				} catch (Throwable t) {
					instance.isJavaPathNew = true;
				}
			} catch (Throwable t) {
				System.err.println("Instance '" + name + "' is corrupted!");
				t.printStackTrace();
			}
			return instance;
		} catch (Exception ex) {
			System.err.println("Failed to load instance: " + name);
			ex.printStackTrace();
			return null;
		}
	}

	public void saveInstance() {
		try {
			File instanceFile = new File(BC.get() + "launcher" + File.separator + "instances", this.name + ".txt");
			if (!instanceFile.exists()) instanceFile.createNewFile();

			PropertyFile instancesettings = new PropertyFile(instanceFile);

			instancesettings.setProperty("name", this.name);
			instancesettings.setProperty("launchArgs", this.launchArgs);
			instancesettings.setProperty("width", Integer.toString(this.width));
			instancesettings.setProperty("height", Integer.toString(this.height));
			instancesettings.setProperty("proxy", Boolean.toString(this.proxy));
			instancesettings.setProperty("keepopen", Boolean.toString(this.keepopen));
			instancesettings.setProperty("RPC", Boolean.toString(this.RPC));
			instancesettings.setProperty("intelFix", Boolean.toString(this.intelFix));
			instancesettings.setProperty("console", Boolean.toString(this.console));
			instancesettings.setProperty("javaPath", javaPath);
			instancesettings.setProperty("isJavaPathNew", Boolean.toString(this.isJavaPathNew));

			StringBuilder builder = new StringBuilder();
			String addons = "";
			if (this.addons.size() > 0) {
				for (String addon : this.addons) {
					builder.append(addon + ",");
				}
				addons = builder.toString().substring(0, builder.toString().length() - 1);
			}

			instancesettings.setProperty("addons", addons);
			instancesettings.setProperty("gameDir", this.gameDir);
			instancesettings.setProperty("version", this.version);
			instancesettings.flushToDisk();
			System.out.println("Saved instance: " + this.name);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	public void removeInstance() {
		File instanceFile = new File(BC.get() + "launcher" + File.separator + "instances", this.name + ".txt");
		if (instanceFile.exists()) instanceFile.delete();
	}

	public Instance renameInstance(String newName) {
		Instance renamed = this.clone(newName);

		File instanceFile = new File(BC.get() + "launcher" + File.separator + "instances", this.name + ".txt");
		if (instanceFile.exists()) instanceFile.delete();

		File iconFile = new File(BC.get() + "launcher" + File.separator + "instances", this.name + ".png");
		if (iconFile.exists()) iconFile.renameTo(new File(BC.get() + "launcher" + File.separator + "instances", newName + ".png"));

		return renamed;
	}

	public Instance clone(String name) {
		Instance cloned = newInstance(name);
		cloned.launchArgs = this.launchArgs;
		cloned.proxy = this.proxy;
		cloned.keepopen = this.keepopen;
		cloned.height = this.height;
		cloned.width = this.width;
		cloned.RPC = this.RPC;
		cloned.addons = this.addons;
		cloned.gameDir = this.gameDir;
		cloned.version = this.version;
		cloned.javaPath = this.javaPath;
		cloned.console = this.console;
		cloned.intelFix = this.intelFix;
		cloned.isJavaPathNew = this.isJavaPathNew;
		return cloned;
	}

	public String getIconLocation() throws IOException {
		File imgFile = new File(BC.get() + "launcher" + File.separator + "instances", this.name + ".png");
		File defaultImg = new File(BC.get() + "launcher" + File.separator + "default_icon.png");

		if (!imgFile.exists()) {
			Util.copy(this.getClass().getClassLoader().getResourceAsStream("icons/favicon.png"), defaultImg);

			return defaultImg.getAbsolutePath();
		}
		return imgFile.getAbsolutePath();
	}

	public Image getIcon() {
		try {
			File imgFile = new File(BC.get() + "launcher" + File.separator + "instances", this.name + ".png");
			if (!imgFile.exists()) {
				return ImageIO.read(this.getClass().getClassLoader().getResourceAsStream("icons/favicon.png"));
			}
			return ImageIO.read(imgFile).getScaledInstance(32, 32, 16);
		} catch (Exception e2) {
			e2.printStackTrace();

			this.setIcon(null);
			try {
				return ImageIO.read(this.getClass().getClassLoader().getResourceAsStream("icons/favicon.png"));
			} catch (IOException e) { return null; }
		}
	}

	public void setIcon(File path) {
		try {
			File imgFile = new File(BC.get() + "launcher" + File.separator + "instances", this.name + ".png");
			if (path == null) {
				imgFile.delete();
				return;
			}
			Util.copy(path, imgFile);
		} catch (Throwable e2) {
			e2.printStackTrace();
		}
	}

	public void setAddons(List<String> list) {
		this.addons.clear();
		this.addons.addAll(list);
	}

	public void removeAddon(String name) {
		if (this.addons.contains(name))
			this.addons.remove(name);
	}

	public void addAddon(String name) {
		if (this.addons.contains(name)) return;
		this.addons.add(name);
	}

	public static ArrayList<String> getInstances() {
		ArrayList<String> list = new ArrayList<String>();
		File file = new File(BC.get() + "launcher" + File.separator + "instances");

		String[] rawlist = file.list(new FilenameFilter() {
			public boolean accept(File dir, String name) {
				return name.endsWith(".txt");
			}
		});

		for (int i = 0; i < rawlist.length; i++) {
			list.add(rawlist[i].substring(0, rawlist[i].length() - 4));
		}
		return list;
	}

	public static int getCurrentInstanceIndex() {
		ArrayList<String> list = getInstances();
		for (int i = 0; i < list.size(); i++) {
			if (list.get(i).equals(Launcher.currentInstance.name)) {
				return i;
			}
		}
		return -1;
	}

}
