//
//  CarHomeViewController.swift
//  PIP_IOS
//
//  Created by Michael Wang on 5/23/17.
//  Copyright © 2017 GG. All rights reserved.
//

import UIKit

class CarHomeViewController: UIViewController {

    var pos = 0
    
    @IBOutlet weak var car_pos_image: UIImageView!
    
    @IBAction func next_pos_button(_ sender: Any) {

        
         if pos == 0 {
         
            car_pos_image.image = UIImage(named: "carpos_empty.png")
         }
         
         else {
         
            let image_num = pos - 1
            car_pos_image.image = UIImage(named: "carpos_\(image_num).png")
         
         }
        
        pos = (pos + 1 ) % 14
        
    }

    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
}
