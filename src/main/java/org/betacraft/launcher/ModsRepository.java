package org.betacraft.launcher;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

import org.betacraft.launcher.Release.VersionRepository;

import uk.betacraft.auth.CustomResponse;
import uk.betacraft.auth.Request;
import uk.betacraft.auth.RequestUtil;
import uk.betacraft.json.lib.ModCategory;
import uk.betacraft.json.lib.ModObject;

public class ModsRepository extends JFrame implements ActionListener, LanguageElement {

	public static ArrayList<ModCategory> mods = new ArrayList<ModCategory>();

	public static void loadMods() {
		try {
			String modlistjson = (new Request() {

				@Override
				public CustomResponse perform() {
					this.REQUEST_URL = "http://api.betacraft.uk/getmods.jsp";
					return new CustomResponse(RequestUtil.performGETRequest(this));
				}

			}).perform().response;

			ModCategory[] mc = Util.gson.fromJson(modlistjson, ModCategory[].class);
			
			for (ModCategory cat : mc) {
				mods.add(cat);
			}
		} catch (Exception ex) {
			System.err.println("A critical error occurred while loading mod list!");
			ex.printStackTrace();
		}
	}

	public static ModObject getMod(String name) {
		for (ModCategory cat : mods) {
			for (ModObject obj : cat.mods) {
				if (obj.full_name.equalsIgnoreCase(name)) {
					return obj;
				}
			}
		}
		return null;
	}

	static ArrayList<DefaultMutableTreeNode> treenodes;
	static JTree tree;
	//static JList list;
	//static DefaultListModel listModel;
	static JScrollPane listScroller;
	static JButton more_button;
	static JButton LoadButton, CloseButton;
	static JPanel panel;
	static GridBagConstraints constr;

	public ModsRepository() {
		System.out.println("Mods repository window opened.");
		this.setIconImage(Window.img);
		setMinimumSize(new Dimension(282, 386));
		setTitle(Lang.INSTANCE_MODS_REPOSITORY);
		setResizable(true);

		panel = new JPanel();
		panel.setLayout(new GridBagLayout());

		constr = new GridBagConstraints();

		constr.fill = GridBagConstraints.BOTH;
		constr.insets = new Insets(5, 5, 0, 5);
		constr.gridwidth = GridBagConstraints.RELATIVE;
		constr.weightx = 1.0;

		more_button = new JButton(Lang.ADDON_SHOW_INFO);
		more_button.addActionListener(this);

		panel.add(more_button, constr);
		this.add(panel, BorderLayout.NORTH);

		panel = new JPanel();
		panel.setLayout(new GridBagLayout());
		constr.gridy = 2;
		constr.weighty = GridBagConstraints.RELATIVE;
		constr.gridheight = 1;
		constr.insets = new Insets(0, 5, 5, 5);
		LoadButton = new JButton(Lang.LOAD);
		LoadButton.addActionListener(this);
		panel.add(LoadButton, constr);

		constr.gridx = 1;
		CloseButton = new JButton(Lang.CLOSE);
		CloseButton.addActionListener(this);
		panel.add(CloseButton, constr);

		this.add(panel, BorderLayout.SOUTH);

		panel = new JPanel();
		panel.setLayout(new GridBagLayout());

		updateList();

		this.getContentPane().add(panel, BorderLayout.CENTER);
		this.pack();
		setLocationRelativeTo(Window.mainWindow);
		setVisible(true);
	}

	public void update() {
		this.setTitle(Lang.INSTANCE_MODS_REPOSITORY);
		LoadButton.setText(Lang.OPTIONS_OK);
		this.pack();
	}

	protected void updateList() {
		DefaultMutableTreeNode maintreenode = new DefaultMutableTreeNode("root");
		
		for (ModCategory cat : mods) {
			DefaultMutableTreeNode catnode = new DefaultMutableTreeNode(cat.mod_category);
			for (ModObject obj : cat.mods) {
				DefaultMutableTreeNode modnode = new DefaultMutableTreeNode(obj);
				catnode.add(modnode);
			}
			maintreenode.add(catnode);
		}
		tree = new JTree();
		tree.setRootVisible(false);
		tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);

		DefaultTreeModel treemodel = new DefaultTreeModel(maintreenode);
		tree.setModel(treemodel);

		constr.weighty = 1.0;
		constr.gridheight = GridBagConstraints.RELATIVE;
		constr.gridy = 1;

		if (listScroller != null) panel.remove(listScroller);

		listScroller = new JScrollPane(tree);
		listScroller.setWheelScrollingEnabled(true);
		panel.add(listScroller, constr);
	}

	public void saveVersions() {
		TreePath treepath = tree.getSelectionPath();
		if (treepath == null) {
			JOptionPane.showMessageDialog(this, Lang.ERR_NO_SELECTION, "", JOptionPane.WARNING_MESSAGE);
			return;
		}

		DefaultMutableTreeNode node = (DefaultMutableTreeNode) treepath.getLastPathComponent();

		Object o = node.getUserObject();
		if (o != null) {
			if (o instanceof ModObject) {
				ModObject obj = (ModObject) o;
				DownloadResult download = new ReleaseJson(obj.full_name, obj.info_file_url).downloadJson();

				if (download.isOK()) {
					Release.loadVersions(VersionRepository.BETACRAFT);

					Launcher.currentInstance.version = obj.full_name;
					Launcher.setInstance(Launcher.currentInstance);
					Launcher.currentInstance.saveInstance();
				}
			}
		}
	}

	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == LoadButton) {
			saveVersions();
		} else if (e.getSource() == CloseButton) {
			this.dispose();
			Window.modsRepo = null;
		} else if (e.getSource() == more_button) {
			TreePath treepath = tree.getSelectionPath();
			if (treepath == null) {
				JOptionPane.showMessageDialog(this, Lang.ERR_NO_SELECTION, "", JOptionPane.WARNING_MESSAGE);
				return;
			}

			DefaultMutableTreeNode node = (DefaultMutableTreeNode) treepath.getLastPathComponent();

			Object o = node.getUserObject();

			String link = null;

			if (o instanceof String) {
				for (ModCategory modCat : mods) {
					if (modCat.mod_category.equals((String) o) && modCat.mods.length >= 1) {
						link = modCat.mods[0].website;
						break;
					}
				}
			} else if (o instanceof ModObject) {
				ModObject mod = (ModObject) o;

				link = mod.website;
			}

			if (link != null && !link.isEmpty()) {
				Util.openURL(link);
			}
		}
	}
}
