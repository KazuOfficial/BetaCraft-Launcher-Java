package org.betacraft.launcher;

import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListSelectionModel;

public class InstanceList extends JFrame implements LanguageElement {
	static JList list;
	static DefaultListModel listModel;
	static JScrollPane listScroller;

	static JButton newButton, selectButton, delButton;
	static JPanel panel;
	static GridBagConstraints constr;

	public InstanceList() {
		Logger.a("Instances list window opened.");
		this.setIconImage(Window.img);
		this.setMinimumSize(new Dimension(282, 386));

		this.setTitle(Lang.SELECT_INSTANCE_TITLE);
		this.setResizable(true);

		panel = new JPanel();
		panel.setLayout(new GridBagLayout());

		constr = new GridBagConstraints();

		constr.fill = GridBagConstraints.BOTH;
		constr.insets = new Insets(5, 5, 0, 5);
		constr.gridwidth = 2;
		constr.weightx = 1.0;

		newButton = new JButton(Lang.SELECT_INSTANCE_NEW);
		newButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				String newName = JOptionPane.showInputDialog(InstanceList.this, Lang.INSTANCE_NAME, Lang.SELECT_INSTANCE_NEW, JOptionPane.DEFAULT_OPTION);//.showInputDialog(null, Lang.INSTANCE_NAME, "");
				if (newName != null && !newName.equals("")) Instance.newInstance(newName).saveInstance();
				makeList();
				pack();
			}
		});
		panel.add(newButton, constr);

		makeList();

		constr.gridy++;
		constr.weighty = GridBagConstraints.RELATIVE;
		constr.gridwidth = GridBagConstraints.RELATIVE;
		constr.gridheight = 1;
		constr.insets = new Insets(5, 5, 5, 5);

		selectButton = new JButton(Lang.SELECT);
		selectButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				Launcher.setInstance(Instance.loadInstance((String) list.getSelectedValue()));
				setVisible(false);
				Window.instanceList = null;
			}
		});
		panel.add(selectButton, constr);
		
		constr.gridx = 1;
		
		delButton = new JButton(Lang.REMOVE);
		delButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				int result = JOptionPane.showConfirmDialog(InstanceList.this, Lang.INSTANCE_REMOVE_QUESTION, Lang.INSTANCE_REMOVE_TITLE, JOptionPane.YES_NO_OPTION);
				if (result == JOptionPane.YES_OPTION) {
					Launcher.removeInstance((String) list.getSelectedValue());
					setVisible(false);
				}
			}
		});
		panel.add(delButton, constr);

		this.add(panel);
		this.pack();
		this.setLocationRelativeTo(Window.mainWindow);
		this.setVisible(true);
	}

	public void update() {
		this.setTitle(Lang.SELECT_INSTANCE_TITLE);
		selectButton.setText(Lang.OPTIONS_OK);
		this.pack();
	}

	public void makeList() {
		int i = 0;
		int index = 0;
		listModel = new DefaultListModel();
		String current = Launcher.currentInstance.name;
		for (String item : Instance.getInstances()) {
			listModel.addElement(item);
			if (current.equals(item)) {
				index = i;
			}
			i++;
		}

		constr.weighty = 1.0;
		constr.gridheight = GridBagConstraints.RELATIVE;
		constr.gridy = 1;

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
}