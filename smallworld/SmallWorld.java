/*
	SmallWorld -- Little Smalltalk in Java
		runs as application
	Written by Tim Budd, budd@acm.org
	November 2004

	Version 0.9 November 2004
	Version 0.8 November 2002
*/

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.applet.*;
import javax.swing.*;

public class SmallWorld extends JFrame {
	static public void main (String [] args) {
		world = new SmallWorld(args);
	}

	private static SmallWorld world;
	private SmallInterpreter theInterpreter = new SmallInterpreter();

	public SmallWorld(String [ ] args) {

		setTitle("Small World");
		setSize(200, 150);
		addWindowListener(new WindowAdapter ( ){
			public void windowClosing (WindowEvent e) 
				{ System.exit(0); } });

		JPanel p = new JPanel();
		getContentPane().add(p);
		p.setLayout(new GridLayout(4, 1));
		JButton browserButton = new JButton("class browser");
		browserButton.addActionListener(
			new doItListener("Class browser"));
		p.add(browserButton);
		JButton saveButton = new JButton("save image");
		saveButton.addActionListener(
			new doItListener("File saveImage: 'image'"));
		p.add(saveButton);
		JButton quitButton = new JButton("quit");
		p.add(quitButton);
		quitButton.addActionListener(
		new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				// maybe later do something more intelligent
				System.exit(0);
			}
		});
		p.add(output);

		// now read the image
		show();
		output.setText("Initializing image: wait ...."); repaint();
		if (args.length > 0)
			readImage(args[0]);
		else
			readImage("image");
		repaint();
	}

	private JTextField output = new JTextField();
	
	private void readImage(String name) {
		try {
			ObjectInputStream ois = new ObjectInputStream(new FileInputStream(name));
			theInterpreter = new SmallInterpreter();
			//theInterpreter = (SmallInterpreter) ois.readObject();
			// now read object by object
			theInterpreter.nilObject = (SmallObject) ois.readObject();
			theInterpreter.trueObject = (SmallObject) ois.readObject();
			theInterpreter.falseObject = (SmallObject) ois.readObject();
			theInterpreter.smallInts = (SmallInt []) ois.readObject();
			theInterpreter.ArrayClass = (SmallObject) ois.readObject();
			theInterpreter.BlockClass = (SmallObject) ois.readObject();
			theInterpreter.ContextClass = (SmallObject) ois.readObject();
			theInterpreter.IntegerClass = (SmallObject) ois.readObject();
			output.setText("image initialized");
			done = true;
		} catch (Exception e) {
			output.setText("received I/O exception " + e);
		}
	}

	private boolean done = false;

	private class doItListener implements ActionListener {
		public doItListener(String t) { task = t;}
		private String task;
		public void actionPerformed(ActionEvent e) {
		if (! done) return; // not ready yet
		output.setText(task);
		// start from the basics
		SmallObject TrueClass = theInterpreter.trueObject.objClass;
		SmallObject name = TrueClass.data[0]; // a known string
		SmallObject StringClass = name.objClass;
		// now look for the method
		SmallObject methods = StringClass.data[2];
		SmallObject doItMeth = null;
		for (int i = 0; i < methods.data.length; i++) {
		   SmallObject aMethod = methods.data[i];
		   if ("doIt".equals(aMethod.data[0].toString()))
		     doItMeth = aMethod;
		}
		if (doItMeth == null)
			System.out.println("can't find do it!!");
		else {
			SmallByteArray rec = new SmallByteArray(StringClass, task);
			SmallObject args = new SmallObject(theInterpreter.ArrayClass, 1);
			args.data[0] = rec;
			SmallObject ctx = theInterpreter.buildContext(theInterpreter.nilObject, args, doItMeth);
			try {
			theInterpreter.execute(ctx, null, null);
			} catch(Exception ex) {
System.out.println("caught exeception " + ex);
			}
		}
		}
	}
}
