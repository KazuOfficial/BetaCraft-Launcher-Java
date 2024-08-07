package org.betacraft.launcher;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.text.JTextComponent;

import org.betacraft.launcher.InstanceSettings.OptionsPanel;

import uk.betacraft.auth.Credentials;
import uk.betacraft.auth.MicrosoftAuth;
import uk.betacraft.auth.jsons.microsoft.CheckTokenRequest;
import uk.betacraft.auth.jsons.microsoft.CheckTokenResponse;

public class AwaitingMSALogin extends JFrame {
	private String usercode;
	private String devcode;
	private String url;
	private long pass;
	private int interval;

	public CheckThread checkThread;
	
	public AwaitingMSALogin(String url, String userCode, String devCode, long time, int interval) {
		this.url = url;
		this.usercode = userCode;
		this.devcode = devCode;
		this.pass = System.currentTimeMillis() + time*1000;
		this.interval = interval;
		
		this.setIconImage(Window.img);
		setTitle(Lang.LOGIN_MICROSOFT_TITLE);
		setResizable(true);
		this.setMinimumSize(new Dimension(360, 200));

		GridBagConstraints constr = new GridBagConstraints();

		constr.gridx = 0;
		constr.gridy = 0;
		constr.gridwidth = 3;
		constr.weightx = 0.0;
		constr.fill = GridBagConstraints.HORIZONTAL;
		constr.insets = new Insets(10, 10, 0, 10);
		
		JPanel panel = new OptionsPanel();
		panel.setLayout(new GridBagLayout());

		JLabel label1 = new JLabel(Lang.LOGIN_MICROSOFT_CODE_LINE1);
		label1.setForeground(Color.LIGHT_GRAY);
		JTextField tf = new JTextField(this.url);
		tf.setEditable(false);
		tf.addMouseListener(autofocus);
		JLabel label2 = new JLabel(Lang.LOGIN_MICROSOFT_CODE_LINE2);
		label2.setForeground(Color.LIGHT_GRAY);

		JTextPane tp = new JTextPane();
		tp.setEditable(false);
		tp.setText(this.usercode);
		tp.setOpaque(false);
		tp.setForeground(Color.LIGHT_GRAY);
		tp.setBorder(null);
		// Courier New seems to be available across all supported platforms
		tp.setFont(new Font("Courier New", Font.BOLD, 20));
		tp.setDisabledTextColor(Color.WHITE);
		tp.addMouseListener(autofocus);

		panel.add(label1, constr);
		constr.gridy++;
		constr.weightx = 1.0;
		panel.add(tf, constr);
		constr.weightx = 0.0;
		constr.gridy++;
		panel.add(label2, constr);
		constr.gridy++;
		panel.add(tp, constr);
		
		
		JPanel cancel = new JPanel();
		cancel.setLayout(new GridBagLayout());

		constr.gridy++;
		constr.gridx = 0;
		constr.insets = new Insets(2, 2, 2, 2);
		constr.gridheight = 1;
		constr.weightx = 1.0;

		JButton cancelButton = new JButton(Lang.CANCEL);
		cancelButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				if (checkThread != null) checkThread.interrupt();
				dispose();
			}
		});

		cancelButton.setBackground(Color.WHITE);
		cancel.add(cancelButton, constr);
		
		this.getContentPane().add(panel, BorderLayout.CENTER);
		this.getContentPane().add(cancel, BorderLayout.PAGE_END);
		this.pack();
		setLocationRelativeTo(Window.mainWindow);
		setVisible(true);

		checkThread = new CheckThread();
		checkThread.start();
		
		this.addWindowListener(new java.awt.event.WindowAdapter() {
			@Override
			public void windowClosing(java.awt.event.WindowEvent windowEvent) {
				if (checkThread != null) checkThread.interrupt();
				dispose();
			}
		});
	}

	public class CheckThread extends Thread {
		public void run() {
			try {
				while (pass > System.currentTimeMillis()) {
					CheckTokenResponse ctr = new CheckTokenRequest(devcode, null).perform();
					if (ctr != null) {
						if (ctr.error != null) {
							System.out.println("MSA Error: " + ctr.error);
							System.out.println("Err msg: " + ctr.error_description);
							System.out.println("Err codes: ");
							for (int code : ctr.error_codes) {
								System.out.print(code + " ");
							}
						} else {
							Credentials cred = new Credentials();
							cred.refresh_token = ctr.refresh_token;
							
							MicrosoftAuth msa = new MicrosoftAuth(cred);
							
							Window.continueMSA(msa);
							break;
						}
					}

					Thread.sleep(interval * 1000); // sleep
				}

				dispose();
			} catch (Throwable t) {
				t.printStackTrace();
			}
		}
	}

	public static MouseListener autofocus = new MouseListener() {
		public void mouseClicked(MouseEvent e) {
			if (e.getSource() instanceof JTextComponent) {
				((JTextComponent)e.getSource()).selectAll();
			}
		}

		public void mousePressed(MouseEvent e) {}
		public void mouseReleased(MouseEvent e) {}
		public void mouseEntered(MouseEvent e) {}
		public void mouseExited(MouseEvent e) {}
	};
}
