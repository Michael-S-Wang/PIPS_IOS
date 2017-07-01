//
//  ServiceTableViewCell.swift
//  BLEScanner
//
//  Created by Harry Goodwin on 18/07/2016.
//  Copyright © 2016 GG. All rights reserved.
//

import UIKit

class ServiceTableViewCell: UITableViewCell {

	@IBOutlet weak var serviceNameLabel: UILabel!
	@IBOutlet weak var serviceCharacteristicsButton: UIButton!
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
	}
	
	@IBAction func characteristicsButtonPressed(_ sender: AnyObject) {
	}
}
