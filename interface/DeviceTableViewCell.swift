//
//  DeviceTableViewCell.swift
//  BLEScanner
//
//  Created by Harry Goodwin on 10/07/2016.
//  Copyright © 2016 GG. All rights reserved
//

import UIKit
import CoreBluetooth


class DeviceTableViewCell: UITableViewCell {

    @IBOutlet weak var deviceNodeID: UILabel!
	@IBOutlet weak var deviceRssiLabel: UILabel!
		
	var displayPeripheral: DisplayPeripheral? {
		didSet {

            if let node_id = displayPeripheral!.node_id {
                deviceNodeID.text = "\(node_id)"
            }
            
			if let rssi = displayPeripheral!.lastRSSI {
				deviceRssiLabel.text = "\(rssi)dB"
			}
		}
	}
}
