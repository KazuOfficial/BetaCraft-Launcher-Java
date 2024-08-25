package org.betacraft.launcher;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.util.ArrayList;
import java.util.HashMap;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

import org.betacraft.launcher.VersionSorter.Order;

public class SelectAddons extends JFrame implements ActionListener, LanguageElement {

	static JScrollPane listScroller;
	static JButton OK;
	static Order order = Order.FROM_OLDEST;
	static JPanel panel;
	static GridBagConstraints constr;

	public SelectAddons() {
		System.out.println("Addons list window opened.");
		this.setIconImage(Window.img);
		this.setMinimumSize(new Dimension(282, 386));
		this.setPreferredSize(new Dimension(282, 386));
		this.setTitle(Lang.ADDON_LIST_TITLE);
		this.setResizable(true);
		this.setLayout(new BorderLayout());

		makeList(false);
		updateList();

		this.add(panel, BorderLayout.CENTER);
		this.pack();
		this.setLocationRelativeTo(Window.mainWindow);
		this.setVisible(true);
	}

	public void update() {
		this.setTitle(Lang.ADDON_LIST_TITLE);
		OK.setText(Lang.OPTIONS_OK);
		this.pack();
	}

	protected static HashMap<JCheckBox, String> checkboxes = new HashMap<JCheckBox, String>();

	protected void makeList(boolean addoninfo) {

		panel = new JPanel();
		panel.setLayout(new GridBagLayout());

		constr = new GridBagConstraints();
		constr.fill = GridBagConstraints.BOTH;
		constr.insets = new Insets(5, 5, 0, 5);
		constr.gridwidth = GridBagConstraints.RELATIVE;
		constr.weightx = 1.0;

		panel = new JPanel();
		panel.setLayout(new GridBagLayout());

		constr.gridy = 2;
		constr.weighty = GridBagConstraints.RELATIVE;
		constr.gridheight = 1;
		constr.insets = new Insets(0, 5, 5, 5);
		OK = new JButton(Lang.OPTIONS_OK);
		OK.addActionListener(this);
		panel.add(OK, constr);

		this.add(panel, BorderLayout.SOUTH);

		panel = new JPanel();
		panel.setLayout(new GridBagLayout());
	}

	protected void updateList() {
		checkboxes.clear();
		JPanel listpanel = new JPanel();
		listpanel.setLayout(new GridBagLayout());

		GridBagConstraints constr1 = new GridBagConstraints();
		constr1.gridx = 0;
		constr1.gridy = 0;
		constr1.fill = GridBagConstraints.HORIZONTAL;
		constr1.weightx = 0.0;
		constr1.insets = new Insets(5, 5, 0, 5);

		for (final Addon item : Addon.addons.values()) {
			final JCheckBox checkbox = new JCheckBox();

			for (String addon : Launcher.currentInstance.addons) {
				if (addon.equals(item.name))
					checkbox.setSelected(true);
			}

			JLabel label = new JLabel(item.name);
			label.addMouseListener(new MouseListener() {

				public void mouseClicked(MouseEvent e) {}
				public void mouseEntered(MouseEvent e) {}
				public void mouseExited(MouseEvent e) {}
				public void mousePressed(MouseEvent e) {}
				public void mouseReleased(MouseEvent e) {
					checkbox.setSelected(!checkbox.isSelected());
				}
			});

			JButton aboutButton = new JButton(Lang.ADDON_SHOW_INFO);
			aboutButton.addActionListener(new ActionListener() {

				public void actionPerformed(ActionEvent arg0) {
					Addon a = Addon.addons.get(item.name);
					new BrowserWindow(a.getInfo());
				}
			});

			listpanel.add(checkbox, constr1);
			constr1.gridx = 1;
			constr1.weightx = 1.0;
			listpanel.add(label, constr1);
			constr1.gridx = 2;
			constr1.weightx = 0.0;
			listpanel.add(aboutButton, constr1);
			constr1.gridx = 0;

			checkboxes.put(checkbox, item.name);
			constr1.gridy++;
		}

		constr.weighty = 1.0;
		constr.gridheight = GridBagConstraints.RELATIVE;
		constr.gridy = 1;
		constr.insets = new Insets(5, 5, 5, 5);

		if (listScroller != null) panel.remove(listScroller);

		listScroller = new JScrollPane(listpanel);
		listScroller.setWheelScrollingEnabled(true);
		listScroller.getVerticalScrollBar().setUnitIncrement(10);
		panel.add(listScroller, constr);
	}

	public void saveAddons() {
		ArrayList<String> elist = new ArrayList<String>();
		for (JCheckBox checkbox : checkboxes.keySet()) {
			String name = checkboxes.get(checkbox);
			if (checkbox.isSelected()) {
				elist.add(name);
			}
		}
		Launcher.currentInstance.setAddons(elist);
		dispose();
		Launcher.currentInstance.saveInstance();
	}

	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == OK) {
			saveAddons();
			Window.addonsList = null;
		}
	}
}
