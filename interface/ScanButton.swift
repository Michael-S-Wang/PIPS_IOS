//
//  ScanButton.swift
//  BLEScanner
//
//  Created by Harry Goodwin on 18/07/2016.
//  Copyright © 2016 GG. All rights reserved.
//

import UIKit

class ScanButton: UIButton {
	
	required init?(coder aDecoder: NSCoder){
		super.init(coder: aDecoder)
		
		layer.borderWidth = 1.5
		//layer.borderColor = UIColor.bluetoothBlueColor().cgColor
        layer.borderColor = UIColor.gray.cgColor
	}
	
	func buttonColorScheme(_ isScanning: Bool){
		let title = isScanning ? "Stop Scanning" : "Start Scanning"
		setTitle(title, for: UIControlState())
		
		/*
        let titleColor = isScanning ? UIColor.bluetoothBlueColor() : UIColor.white
		setTitleColor(titleColor, for: UIControlState())
		backgroundColor = isScanning ? UIColor.clear : UIColor.bluetoothBlueColor()
        */

        let titleColor = isScanning ? UIColor.gray : UIColor.white
        setTitleColor(titleColor, for: UIControlState())
        backgroundColor = isScanning ? UIColor.clear : UIColor.gray

    }
}
