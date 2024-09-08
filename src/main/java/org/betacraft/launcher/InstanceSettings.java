package org.betacraft.launcher;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Image;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

public class InstanceSettings extends JFrame implements LanguageElement {

	public JCheckBox proxyCheck;
	public JCheckBox keepOpenCheck;
	public JCheckBox RPCCheck;
	public JCheckBox intelFix;
	public JCheckBox showConsole;
	public JCheckBox forceUpdate = null;

	public JLabel parametersText, gameDirText, javaPathText;
	public JTextField parameters;

	public JLabel dimensions1Text;
	public JLabel dimensions2Text;
	public JTextField dimensions1;
	public JTextField dimensions2;

	public JButton OKButton;

	public JTextField dirPath, javaPath;
	public JButton dirChooser, javaChooser;
	public JTextField instanceName;
	public JLabel instanceIcon;
	public JButton chooseIcon;
	public JLabel removelabel;
	public JLabel instanceNameText;
	public JButton addons;
	public JButton modrepo;
	static Image img = null;
	static Image image = null;

	protected static class OptionsPanel extends JPanel {
		public OptionsPanel() {
			try {
				image = ImageIO.read(Launcher.class.getResource("/icons/stone.png")).getScaledInstance(32, 32, 16);
			} catch (IOException e2) {
				e2.printStackTrace();
				return;
			}
		}

		@Override
		protected void paintComponent(Graphics g) {
			final int w = this.getWidth() / 2;
			final int h = this.getHeight() / 2;
			if (w <= 0 || h <= 0) return;
			img = this.createImage(w, h);
			final Graphics graphics2 = img.getGraphics();
			for (int i = 0; i <= w / 32; ++i) {
				for (int j = 0; j <= h / 32; ++j) {
					graphics2.drawImage(image, i * 32, j * 32, null);
				}
			}
			graphics2.dispose();
			g.drawImage(img, 0, 0, w * 2, h * 2, null);
		}
	}

	public InstanceSettings() {
		System.out.println("Options window opened.");

		this.setIconImage(Window.img);
		setTitle(Lang.OPTIONS_TITLE);
		setResizable(true);
		setLayout(new BorderLayout());


		JPanel panel = new OptionsPanel();
		panel.setLayout(new GridBagLayout());

		GridBagConstraints constr = new GridBagConstraints();

		constr.gridx = 0;
		constr.gridy = 0;
		constr.fill = GridBagConstraints.BOTH;
		constr.gridwidth = 4;
		constr.weightx = 1.0;
		constr.insets = new Insets(10, 10, 0, 10);

		proxyCheck = new JCheckBox(Lang.OPTIONS_PROXY);
		proxyCheck.setForeground(Color.LIGHT_GRAY);
		proxyCheck.setOpaque(false);
		proxyCheck.setSelected(Launcher.currentInstance.proxy);
		panel.add(proxyCheck, constr);

		constr.insets = new Insets(2, 10, 0, 10);
		constr.gridy++;
		keepOpenCheck = new JCheckBox(Lang.OPTIONS_KEEP_OPEN);
		keepOpenCheck.setForeground(Color.LIGHT_GRAY);
		keepOpenCheck.setOpaque(false);
		keepOpenCheck.setSelected(Launcher.currentInstance.keepopen);
		panel.add(keepOpenCheck, constr);

		constr.gridy++;
		RPCCheck = new JCheckBox(Lang.OPTIONS_RPC);
		RPCCheck.setForeground(Color.LIGHT_GRAY);
		RPCCheck.setOpaque(false);
		RPCCheck.setSelected(Launcher.currentInstance.RPC);
		panel.add(RPCCheck, constr);
		
		constr.gridy++;
		intelFix = new JCheckBox(Lang.OPTIONS_INTEL_TRICK);
		intelFix.setForeground(Color.LIGHT_GRAY);
		intelFix.setOpaque(false);
		intelFix.setSelected(Launcher.currentInstance.intelFix);
		if (OS.isWindows()) {
			panel.add(intelFix, constr);
		}

		constr.gridy++;
		parametersText = new JLabel(Lang.OPTIONS_LAUNCH_ARGS);
		parametersText.setForeground(Color.LIGHT_GRAY);
		panel.add(parametersText, constr);

		constr.gridy++;
		constr.insets = new Insets(2, 20, 0, 10);
		parameters = new JTextField(Launcher.currentInstance.launchArgs, 30);
		panel.add(parameters, constr);
		
		constr.gridy++;
		constr.insets = new Insets(2, 10, 0, 10);
		gameDirText = new JLabel(Lang.INSTANCE_DIRECTORY);
		gameDirText.setForeground(Color.LIGHT_GRAY);
		panel.add(gameDirText, constr);

		constr.gridy++;
		constr.fill = GridBagConstraints.HORIZONTAL;
		constr.gridwidth = 3;
		constr.insets = new Insets(2, 20, 0, 10);
		
		dirPath = new JTextField(Launcher.currentInstance.gameDir, 4);
		panel.add(dirPath, constr);
		
		constr.gridx += 3;
		constr.weightx = 0.0;
		constr.gridwidth = 1;
		constr.insets = new Insets(2, 10, 0, 10);
		
		dirChooser = new JButton(Lang.BROWSE);
		dirChooser.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				JFileChooser dirChooser = new JFileChooser();
				dirChooser.setCurrentDirectory(new java.io.File(BC.get()));
				dirChooser.setDialogTitle(Lang.INSTANCE_GAME_DIRECTORY_TITLE);
				dirChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
				dirChooser.setAcceptAllFileFilterUsed(false);
				if (dirChooser.showOpenDialog(Window.instanceSettings) == JFileChooser.APPROVE_OPTION) { 
					System.out.println("getCurrentDirectory(): " 
							+  dirChooser.getCurrentDirectory());
					System.out.println("getSelectedFile() : " 
							+  dirChooser.getSelectedFile());
					File gameDir = null;
					if (!dirChooser.getSelectedFile().equals(dirChooser.getCurrentDirectory())) {
						if (!dirChooser.getSelectedFile().isDirectory()) {
							gameDir = dirChooser.getCurrentDirectory();
						} else {
							gameDir = dirChooser.getSelectedFile();
						}
					}
					dirPath.setText(gameDir.getAbsolutePath());
				}
			}
		});
		panel.add(dirChooser, constr);
		
		
		
		constr.gridx = 0;
		constr.gridy++;
		javaPathText = new JLabel(Lang.JAVA_EXECUTABLE);
		javaPathText.setForeground(Color.LIGHT_GRAY);
		panel.add(javaPathText, constr);
		
		constr.gridy++;
		constr.gridx = 0;
		constr.gridwidth = 3;
		constr.weightx = 1.0;
		constr.insets = new Insets(2, 20, 0, 10);
		
		javaPath = new JTextField(Launcher.currentInstance.javaPath, 4);
		panel.add(javaPath, constr);
		
		constr.gridx += 3;
		constr.weightx = 0.0;
		constr.gridwidth = 1;
		constr.insets = new Insets(2, 10, 0, 10);
		
		javaChooser = new JButton(Lang.BROWSE);
		javaChooser.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				JFileChooser dirChooser = new JFileChooser();
				dirChooser.setCurrentDirectory(new java.io.File(BC.get()));
				dirChooser.setDialogTitle(Lang.JAVA_EXECUTABLE);
				dirChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);

				if (dirChooser.showOpenDialog(Window.instanceSettings) == JFileChooser.APPROVE_OPTION) {
					File gameDir = dirChooser.getSelectedFile();
					if (gameDir.getAbsolutePath().endsWith(".plugin") && OS.isMac()) {
					    gameDir = new File(gameDir, "Contents/Home/bin/java");
					}
					javaPath.setText(gameDir.getAbsolutePath());
				}
			}
		});
		panel.add(javaChooser, constr);

		constr.fill = GridBagConstraints.HORIZONTAL;
		constr.gridwidth = 1;
		constr.gridx = 0;
		constr.weightx = 0.0;
		constr.insets = new Insets(10, 10, 10, 10);
		constr.gridy++;

		// separate panel for resolution fields
		JPanel resPanel = new OptionsPanel();
		resPanel.setLayout(new javax.swing.BoxLayout(resPanel, BoxLayout.X_AXIS));

		dimensions1Text = new JLabel(Lang.OPTIONS_WIDTH);
		dimensions1Text.setForeground(Color.LIGHT_GRAY);
		resPanel.add(dimensions1Text);
		resPanel.add(Box.createRigidArea(new Dimension(5, 0)));

		dimensions1 = new JTextField(Integer.toString(Launcher.currentInstance.width), 4);
		resPanel.add(dimensions1);
		resPanel.add(Box.createRigidArea(new Dimension(20, 0)));

		dimensions2Text = new JLabel(Lang.OPTIONS_HEIGHT);
		dimensions2Text.setForeground(Color.LIGHT_GRAY);
		resPanel.add(dimensions2Text);
		resPanel.add(Box.createRigidArea(new Dimension(5, 0)));

		dimensions2 = new JTextField(Integer.toString(Launcher.currentInstance.height), 4);
		resPanel.add(dimensions2);
		panel.add(resPanel, constr);

		JPanel instanceSettings = new OptionsPanel();
		instanceSettings.setLayout(new GridBagLayout());
		JButton remove = new JButton("x");

		remove.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				int result = JOptionPane.showConfirmDialog(InstanceSettings.this, Lang.INSTANCE_REMOVE_QUESTION, Lang.INSTANCE_REMOVE_TITLE, JOptionPane.YES_NO_OPTION);
				if (result == JOptionPane.YES_OPTION) {
					Launcher.removeInstance(Launcher.currentInstance.name);
					setVisible(false);
				}
			}
		});

		removelabel = new JLabel(Lang.INSTANCE_REMOVE_TITLE);

		instanceNameText = new JLabel(Lang.INSTANCE_NAME);
		instanceIcon = new JLabel(new ImageIcon(Launcher.currentInstance.getIcon()));

		chooseIcon = new JButton(Lang.INSTANCE_CHANGE_ICON_NAME);
		chooseIcon.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				JFileChooser dirChooser = new JFileChooser();
				dirChooser.setCurrentDirectory(new java.io.File(System.getProperty("user.home")));
				dirChooser.setDialogTitle(Lang.INSTANCE_CHANGE_ICON_TITLE);
				dirChooser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
				dirChooser.setAcceptAllFileFilterUsed(false);
				if (dirChooser.showOpenDialog(Window.mainWindow) == JFileChooser.APPROVE_OPTION) { 
					File selected = dirChooser.getSelectedFile();
					try {
						Launcher.currentInstance.setIcon(selected);
						instanceIcon.setIcon(new ImageIcon(Launcher.currentInstance.getIcon()));
					} catch (Exception ex) {
						Launcher.currentInstance.setIcon(null);
						JOptionPane.showMessageDialog(Window.mainWindow, String.format(Lang.INSTANCE_CHANGE_ICON_FAILED, ex.getMessage()), Lang.INSTANCE_CHANGE_ICON_FAILED_TITLE, JOptionPane.ERROR_MESSAGE);
					}
				}
			}
		});

		addons = new JButton(Lang.INSTANCE_SELECT_ADDONS);
		addons.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				if (Window.addonsList == null) new SelectAddons();
				else Window.addonsList.setVisible(true);
			}
		});

		modrepo = new JButton(Lang.INSTANCE_MODS_REPOSITORY);
		modrepo.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				if (Window.modsRepo == null) new ModsRepository();
				else Window.modsRepo.setVisible(true);
			}
		});

		instanceName = new JTextField(Launcher.currentInstance.name, 15);
		instanceNameText.setForeground(Color.LIGHT_GRAY);
		instanceNameText.setOpaque(false);

		removelabel.setForeground(Color.LIGHT_GRAY);
		removelabel.setOpaque(false);
		instanceName.setOpaque(true);

		GridBagConstraints constr1 = new GridBagConstraints();
		constr1.fill = GridBagConstraints.HORIZONTAL;
		constr1.insets = new Insets(2, 2, 10, 10);
		constr1.gridy = 0;
		constr1.gridx = 0;
		instanceSettings.add(remove, constr1);

		constr1.gridx = 1;
		instanceSettings.add(removelabel, constr1);

		constr1.fill = GridBagConstraints.NORTHEAST;
		constr1.gridx = 0;
		constr1.gridy = 1;
		instanceSettings.add(instanceNameText, constr1);

		constr1.gridx = 1;
		instanceSettings.add(instanceName, constr1);

		constr1.insets = new Insets(10, 2, 2, 2);
		constr1.gridx = 0;
		constr1.gridy = 2;
		instanceSettings.add(instanceIcon, constr1);

		constr1.gridx = 1;
		constr1.gridy = 2;
		constr1.gridwidth = 3;
		constr1.ipadx = 50;
		instanceSettings.add(chooseIcon, constr1);

		constr1.gridx = 0;
		constr1.gridy = 3;
		constr1.weightx = 1.0;
		constr1.ipadx = 0;
		constr1.insets = new Insets(25, 2, 2, 2);
		constr1.fill = GridBagConstraints.HORIZONTAL;
		
		showConsole = new JCheckBox(Lang.CONSOLE_OUTPUT);
		showConsole.setForeground(Color.LIGHT_GRAY);
		showConsole.setOpaque(false);
		showConsole.setSelected(Launcher.currentInstance.console);
		instanceSettings.add(showConsole, constr1);
		
		constr1.gridy++;
		constr1.insets = new Insets(2, 2, 2, 2);
		forceUpdate = new JCheckBox(Lang.FORCE_UPDATE);
		forceUpdate.setForeground(Color.LIGHT_GRAY);
		forceUpdate.setOpaque(false);
		forceUpdate.setSelected(Launcher.forceUpdate);
		instanceSettings.add(forceUpdate, constr1);

		constr1.gridy++;
		instanceSettings.add(addons, constr1);

		constr1.ipady = 0;
		constr1.gridy++;
		instanceSettings.add(modrepo, constr1);

		JPanel okPanel = new OptionsPanel();
		okPanel.setLayout(new GridBagLayout());

		constr1.fill = GridBagConstraints.HORIZONTAL;
		constr1.gridx = 0;
		constr1.gridy = constr.gridy + 1;
		constr1.weighty = 1.0;
		constr1.insets = new Insets(2, 2, 2, 2);
		constr1.gridwidth = GridBagConstraints.RELATIVE;
		constr1.gridheight = 1;
		constr1.weightx = 1.0;
		OKButton = new JButton(Lang.OPTIONS_OK);
		OKButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				if (saveOptions()) {
					dispose();
					Window.instanceSettings = null;
				}
			}
		});
		okPanel.add(OKButton, constr1);
		okPanel.setBackground(Color.WHITE);

		this.getContentPane().add(panel, BorderLayout.CENTER);
		this.getContentPane().add(instanceSettings, BorderLayout.LINE_START);
		this.getContentPane().add(okPanel, BorderLayout.PAGE_END);
		this.pack();
		setLocationRelativeTo(Window.mainWindow);
		setVisible(true);

		DocumentListener doc = new DocumentListener() {

			public void changedUpdate(DocumentEvent e) {
				change();
			}
			public void removeUpdate(DocumentEvent e) {
				change();
			}
			public void insertUpdate(DocumentEvent e) {
				change();
			}

			public void change() {
				if (dimensions1.getText().length() > 9) {
					Window.setTextInField(dimensions1, "");
				}
				if (dimensions2.getText().length() > 9) {
					Window.setTextInField(dimensions2, "");
				}
				for (int i = 0; i < dimensions1.getText().length(); i++) {
					if ("0123456789".indexOf(dimensions1.getText().charAt(i)) < 0) {
						Window.setTextInField(dimensions1, "");
					}
				}
				for (int i = 0; i < dimensions2.getText().length(); i++) {
					if ("0123456789".indexOf(dimensions2.getText().charAt(i)) < 0) {
						Window.setTextInField(dimensions2, "");
					}
				}
			}
		};
		dimensions1.getDocument().addDocumentListener(doc);
		dimensions2.getDocument().addDocumentListener(doc);

		this.setMinimumSize(this.getPreferredSize().getSize());
		OKButton.requestFocus();
	}

	public void update() {
		this.setTitle(Lang.OPTIONS_TITLE);
		proxyCheck.setText(Lang.OPTIONS_PROXY);
		keepOpenCheck.setText(Lang.OPTIONS_KEEP_OPEN);
		RPCCheck.setText(Lang.OPTIONS_RPC);
		intelFix.setText(Lang.OPTIONS_INTEL_TRICK);
		parametersText.setText(Lang.OPTIONS_LAUNCH_ARGS);
		dirChooser.setText(Lang.BROWSE);
		dimensions1Text.setText(Lang.OPTIONS_WIDTH);
		dimensions2Text.setText(Lang.OPTIONS_HEIGHT);
		removelabel.setText(Lang.INSTANCE_REMOVE_TITLE);
		instanceNameText.setText(Lang.INSTANCE_NAME);
		chooseIcon.setText(Lang.INSTANCE_CHANGE_ICON_NAME);
		addons.setText(Lang.INSTANCE_SELECT_ADDONS);
		modrepo.setText(Lang.INSTANCE_MODS_REPOSITORY);
		forceUpdate.setText(Lang.FORCE_UPDATE);
		OKButton.setText(Lang.OPTIONS_OK);
		this.setMinimumSize(this.getPreferredSize().getSize());
		this.pack();
	}

	public static String checkJava(String jpath) {
		int i = Util.getMajorJavaVersion(jpath);
		if (i == -1) {
			int res = JOptionPane.showConfirmDialog(Window.instanceSettings, Lang.JAVA_INVALID, "",
					JOptionPane.YES_NO_OPTION);
			if (res == JOptionPane.YES_OPTION) {
				// user has chosen to use recommended java
				String recommended = Util.findRecommendedJava();
				if (recommended == null) {
					if (JOptionPane.showConfirmDialog(Window.instanceSettings, Lang.JAVA_RECOMMENDED_NOT_FOUND, "",
							JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
						new SimpleWebAddressFrame("https://www.java.com/download/manual.jsp");
					}
					return null;
				}
				return recommended;
			} else {
				return null;
			}
		} else if (i > 8) {
			int res = JOptionPane.showConfirmDialog(Window.instanceSettings, Lang.JAVA_TOO_RECENT, "",
					JOptionPane.YES_NO_OPTION);
			if (res == JOptionPane.YES_OPTION) {
				// user has chosen to use recommended java
				String recommended = Util.findRecommendedJava();
				if (recommended == null) {
					if (JOptionPane.showConfirmDialog(Window.instanceSettings, Lang.JAVA_RECOMMENDED_NOT_FOUND, "",
							JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION) {
						new SimpleWebAddressFrame("https://www.java.com/download/manual.jsp");
					}
					return null;
				}
				return recommended;
			} else {
				return jpath;
			}
		}
		return jpath;
	}

	public boolean saveOptions() {
		String jpath = javaPath.getText();

		// warn on unrecommended java version
		if (!Launcher.disableWarnings) {
			jpath = checkJava(jpath);

			if (jpath == null)
				return false;
			
			javaPath.setText(jpath);
		}

		Launcher.forceUpdate = forceUpdate.isSelected();
		if (!jpath.equals(Launcher.currentInstance.javaPath)) {
			Launcher.currentInstance.isJavaPathNew = true;
		}
		Launcher.currentInstance.javaPath = jpath;
		try {
			Launcher.currentInstance.width = Integer.parseInt(dimensions1.getText());
		} catch (Exception ex) {
			Launcher.currentInstance.width = 854;
		}
		try {
			Launcher.currentInstance.height = Integer.parseInt(dimensions2.getText());
		} catch (Exception ex) {
			Launcher.currentInstance.height = 480;
		}

		Launcher.currentInstance.gameDir = dirPath.getText();
		Launcher.currentInstance.keepopen = keepOpenCheck.isSelected();
		Launcher.currentInstance.proxy = proxyCheck.isSelected();
		Launcher.currentInstance.RPC = RPCCheck.isSelected();
		Launcher.currentInstance.intelFix = intelFix.isSelected();
		Launcher.currentInstance.console = showConsole.isSelected();
		Launcher.currentInstance.launchArgs = parameters.getText();

		if (!instanceName.getText().equals(Launcher.currentInstance.name) && !instanceName.getText().equals("")) {
			Launcher.setInstance(Launcher.currentInstance.renameInstance(instanceName.getText()));
		}
		Launcher.currentInstance.saveInstance();
		BC.SETTINGS.setProperty("lastInstance", Launcher.currentInstance.name);
		BC.SETTINGS.flushToDisk();
		return true;
	}
}
