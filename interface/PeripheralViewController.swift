//
//  PeripheralTableViewController.swift
//  BLEScanner
//
//  Created by Harry Goodwin on 21/01/2016.
//  Copyright © 2016 GG. All rights reserved.
//

import CoreBluetooth
import UIKit
import MessageUI
import Libpipsapi

struct DisplayPeripheral{
	var node_id: String?
    var lastRSSI: NSNumber?
}

class PeripheralViewController: UIViewController, UITextFieldDelegate, MFMailComposeViewControllerDelegate {
	
	@IBOutlet weak var bluetoothIcon: UIImageView!
	@IBOutlet weak var scanningButton: ScanButton!
    @IBOutlet weak var sendEmail: UIButton!
    
    @IBOutlet weak var rssi_node_1: UILabel!
    @IBOutlet weak var rssi_node_2: UILabel!
    @IBOutlet weak var rssi_node_3: UILabel!
    @IBOutlet weak var rssi_node_4: UILabel!
    
    @IBOutlet weak var log_file_name: UITextField!
    @IBOutlet weak var log_file_comment: UITextField!
    var log_file_name_timestamp: String?
    
    @IBOutlet weak var position_name: UILabel!
    @IBOutlet weak var postion_coordinate: UILabel!
    
    @IBOutlet weak var debug_message: UILabel!
    
    @IBOutlet weak var cmd_1: UITextField!
    @IBOutlet weak var cmd_2: UITextField!
    @IBOutlet weak var cmd_3: UITextField!
    @IBOutlet weak var cmd_4: UITextField!
    @IBOutlet weak var pm_1: UITextField!
    @IBOutlet weak var pm_2: UITextField!
    @IBOutlet weak var pm_3: UITextField!
    @IBOutlet weak var pm_4: UITextField!
    var api_cmd = Libpipsapi.pips_api_cmd()
    
    
    var HTTP_ID = "0"
    var log_on = false
    
    var flag = 0
    
    let BLE_MODE = "beacon-mode"    // beacon-mode or non-beacon-mode
    
    var startTime: TimeInterval = 0
    var endTime: TimeInterval = 0
	
    var centralManager: CBCentralManager?
    var peripherals: [DisplayPeripheral] = []
	var viewReloadTimer: Timer?
	
	var selectedPeripheral: CBPeripheral?
	
	@IBOutlet weak var tableView: UITableView!
	
	required init?(coder aDecoder: NSCoder) {
		super.init(coder: aDecoder)
		
		//Initialise CoreBluetooth Central Manager
		centralManager = CBCentralManager(delegate: self, queue: DispatchQueue.main)
	}
	
	override func viewWillAppear(_ animated: Bool) {
		super.viewWillAppear(animated)
		
		viewReloadTimer = Timer.scheduledTimer(timeInterval: 0.0, target: self, selector: #selector(PeripheralViewController.refreshScanView), userInfo: nil, repeats: true)
	}
	
	override func viewWillDisappear(_ animated: Bool) {
		super.viewWillDisappear(animated)
		
		viewReloadTimer?.invalidate()
	}
	
    override func viewDidLoad() {
        super.viewDidLoad()
        
        log_file_name.delegate = self
        log_file_comment.delegate = self
        
        cmd_1.delegate = self
        cmd_2.delegate = self
        cmd_3.delegate = self
        cmd_4.delegate = self
        pm_1.delegate = self
        pm_2.delegate = self
        pm_3.delegate = self
        pm_4.delegate = self
    }
    
	func updateViewForScanning(){
		bluetoothIcon.pulseAnimation()
		bluetoothIcon.isHidden = false
		scanningButton.buttonColorScheme(true)
	}
	
	func updateViewForStopScanning(){
		let plural = peripherals.count > 1 ? "s" : ""
		bluetoothIcon.layer.removeAllAnimations()
		bluetoothIcon.isHidden = true
		scanningButton.buttonColorScheme(false)
        
        for (index, foundPeripheral) in peripherals.enumerated(){
                print(foundPeripheral)
                print("\n\n")
        }
	}

    @IBAction func cmd_button(_ sender: Any) {
        
        api_cmd.cmd1 = Int32(cmd_1.text!)!
        api_cmd.cmd2 = Int32(cmd_2.text!)!
        api_cmd.cmd3 = Int32(cmd_3.text!)!
        api_cmd.cmd4 = Int32(cmd_4.text!)!
        
        api_cmd.para1 = Double(pm_1.text!)!
        api_cmd.para2 = Double(pm_2.text!)!
        api_cmd.para3 = Double(pm_3.text!)!
        api_cmd.para4 = Double(pm_4.text!)!
    
    
        //print("api_cmd1: \(api_cmd.cmd1)")
        //print("api_pm1: \(api_cmd.para1)")
        
        Libpipsapi.PIPS_api_cmd(&api_cmd)
    
    }

	@IBAction func scanningButtonPressed(_ sender: AnyObject){
		if centralManager!.isScanning{
            print("\n\n******************** STOP BUTTON PUSHED *********************")
			centralManager?.stopScan()
			updateViewForStopScanning()
            
            
            if (self.HTTP_ID != "0") {
                print("HTTP_ID != 0")
                let log_file_comment_str = log_file_comment.text!.components(separatedBy: "\'")
                print("log file comment str: \(log_file_comment_str)")
                //http_post("http:/192.168.1.162:3000/api/Recordings", "elapsed=0&adv_len=0&RSSI=0&adv_data=0&comment=\(log_file_comment_str)")
                //sleep(1)
                //log_on = false
                //http_get("http://192.168.1.162:3000/api/tests/complete?id=\(self.HTTP_ID)", " ")
            }

            let log_file_comment_str = log_file_comment.text!.components(separatedBy: "\'")
            print("log file comment str: \(log_file_comment_str)")
            let str = String(format: "0\t\t0\t\t0\t\t0\t\t\(log_file_comment_str)\n")
            File("WRITE", str)

            
            File("READ", "")
            File("EMAIL", "")
            File("DELETE", "")
            
        
        }else{
            print("\n\n******************** START BUTTON PUSHED *********************")
            
            Libpipsapi.PIPS_rx_init()
            
            // open new logging file
            
            let current_time = Date()
            let dateformat = DateFormatter()
            dateformat.locale = Locale(identifier: "en_US_POSIX")
            dateformat.timeZone = TimeZone(abbreviation: "EST")
            dateformat.dateFormat = "yyyy-MM-dd'T'HH_mm_ss"
            //dateformat.dateFormat = "yyyy-MM-dd'T'HH:mm:ss:SSSZ"
            let current_date_str = dateformat.string(from: current_time)
            //http_post("http://192.168.1.162:3000/api/tests", "Name=\(log_file_name.text!)&date=\(current_date_str)")
            
            log_file_name_timestamp = "\(log_file_name.text!)_\(current_date_str).txt"
            print(log_file_name_timestamp!)

            File("WRITE", "elapsed\t\tadv_len\t\tRSSI\t\tadv_data\t\tcomment\n")
            
 
            startScanning()
        }
	}
	
    func File(_ cmd:String, _ line:String) {

        let dir:NSURL = FileManager.default.urls(for: FileManager.SearchPathDirectory.cachesDirectory, in: FileManager.SearchPathDomainMask.userDomainMask).last! as NSURL
        let fileurl =  dir.appendingPathComponent(log_file_name_timestamp!)
        
        print("\ndir: \(dir)")
        print("file ulr: \(String(describing: fileurl))")
        print("\n")
        
        if cmd == "WRITE" {
            
            print(".......WRITE TO FILE........")
            
            let data = line.data(using: String.Encoding.utf8, allowLossyConversion: false)!
            
            if FileManager.default.fileExists(atPath: fileurl!.path) {
                
                do{
                    let fileHandle = try FileHandle(forWritingTo: fileurl!)
                    
                    print("debug: WRITE if")
                    fileHandle.seekToEndOfFile()
                    fileHandle.write(data)
                    fileHandle.closeFile()
                    
                } catch {
                    print("Can't open fileHandle \(error)")
                }
            } else {
                
                do{
                    print("debug: WRITE else")
                    try data.write(to: fileurl!, options: .atomic)
                    
                } catch {
                    print("Can't write \(error)")
                }
            }
        }
        
        else if cmd == "DELETE" {

            print(".......DELETE FILE........")
   
            let fileManager = FileManager.default
            
            do {
                
                var files = try fileManager.contentsOfDirectory(at: dir as URL, includingPropertiesForKeys: nil)
                print("before files: \(files)\n")
                
                try fileManager.removeItem(at: fileurl!)
                
                // delete other files in dir
                // let fileurl2 =  dir.appendingPathComponent("file_name_2017-05-21T10_20_44.txt")
                // try fileManager.removeItem(at: fileurl2!)

                
                files = try fileManager.contentsOfDirectory(at: dir as URL, includingPropertiesForKeys: nil)
                print("after files: \(files)")
                
            }
            catch let error as NSError {
                print("Ooops! Something went wrong: \(error)")
            }
        }
        
        else if cmd == "READ" {
         
            print(".......READ FROM FILE........")

            do {
                let fileHandle = try FileHandle(forReadingFrom: fileurl!)
                let data = fileHandle.readDataToEndOfFile()
                fileHandle.closeFile()
                
                let str = NSString(data: data, encoding: String.Encoding.utf8.rawValue)
                print(str!)

            } catch {
                print("Can't read \(error)")
            }
        }
        
        else if cmd == "EMAIL" {

            print(".......EMAIL FROM FILE........")
            
            //Check to see the device can send email.
            if( MFMailComposeViewController.canSendMail() ) {
                print("Can send email.")
                
                let mailComposer = MFMailComposeViewController()
                mailComposer.mailComposeDelegate = self
                
                //Set the subject and message of the email
                mailComposer.setSubject(log_file_name_timestamp!)
                mailComposer.setMessageBody(log_file_name_timestamp!, isHTML: false)
                mailComposer.setToRecipients(["jerryshome@gmail.com"])
                
                if let fileData = NSData(contentsOf: fileurl!) {
                    mailComposer.addAttachmentData(fileData as Data, mimeType: "text/txt", fileName: log_file_name_timestamp!)
                }
                
                self.present(mailComposer, animated: true, completion: nil)
            }
        }
    }
    
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        
        self.log_file_name.resignFirstResponder()
        self.log_file_comment.resignFirstResponder()
        
        self.cmd_1.resignFirstResponder()
        self.cmd_2.resignFirstResponder()
        self.cmd_3.resignFirstResponder()
        self.cmd_4.resignFirstResponder()
        self.pm_1.resignFirstResponder()
        self.pm_2.resignFirstResponder()
        self.pm_3.resignFirstResponder()
        self.pm_4.resignFirstResponder()
        
        return true;
    }
    
    func mailComposeController(_ controller: MFMailComposeViewController, didFinishWith result: MFMailComposeResult, error: Error?) {
        self.dismiss(animated: true, completion: nil)
    }
    
	func startScanning(){
        peripherals = []
		self.centralManager?.scanForPeripherals(withServices: nil, options: [CBCentralManagerScanOptionAllowDuplicatesKey: true])
		updateViewForScanning()

        /*
        let triggerTime = (Int64(NSEC_PER_SEC) * 10)
		DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + Double(triggerTime) / Double(NSEC_PER_SEC), execute: { () -> Void in
			if self.centralManager!.isScanning{
				self.centralManager?.stopScan()
				self.updateViewForStopScanning()
			}
		})
        */
	}
	
	func refreshScanView()
	{
		if peripherals.count > 1 && centralManager!.isScanning{
			tableView.reloadData()
		}
	}
}

extension PeripheralViewController: CBCentralManagerDelegate{
	func centralManagerDidUpdateState(_ central: CBCentralManager){
		if (central.state == CBManagerState.poweredOn){
			//startScanning()
		}else{
			// do something like alert the user that ble is not on
		}
	}
    func http_get(_ url:String, _ paramString:String)
    {
        let url:NSURL = NSURL(string: url)!
        let session = URLSession.shared
        
        let request = NSMutableURLRequest(url: url as URL)
        request.httpMethod = "GET"
        
        //let paramString = "x=1&y=1&z=1&theta=1&altitude=1&RSI=100"
        //request.httpBody = paramString.data(using: String.Encoding.utf8)
        
        let task = session.dataTask(with: request as URLRequest) {
            (data, response, error) in
            
            guard let _:NSData = data as NSData?, let _:URLResponse = response, error == nil else {
                print("this error")
                return
            }
            
            if let dataString = NSString(data: data!, encoding: String.Encoding.utf8.rawValue)
            {
                print("\(self.flag): dataString -  \(dataString)")
            }
            
            //call callback here
        }
        
        task.resume()
        
    }
    
    func http_post(_ url:String, _ paramString:String)
    {
        let url:NSURL = NSURL(string: url)!
        let session = URLSession.shared
        
        let request = NSMutableURLRequest(url: url as URL)
        request.httpMethod = "POST"
    
        //let paramString = "x=1&y=1&z=1&theta=1&altitude=1&RSI=100"
        request.httpBody = paramString.data(using: String.Encoding.utf8)
        
        let task = session.dataTask(with: request as URLRequest) {
            (data, response, error) in
            
            guard let _:NSData = data as NSData?, let _:URLResponse = response, error == nil else {
                print("this error")
                return
            }
            
            if let dataString = NSString(data: data!, encoding: String.Encoding.utf8.rawValue)
            {
                print("POST #\(self.flag): dataString -  \(dataString)")
                print("url: \(url)")
                self.flag = self.flag + 1
                
                // set HTTP_ID if POST command is to start log file
                let words = dataString.components(separatedBy: ":")
                print("words : \(words)")
                if words[0] == "{\"Name\"" {
                    self.HTTP_ID = String(words[words.count - 1].characters.dropLast())
                    print("http id: \(self.HTTP_ID)")
                }
            }
            
            //call callback here
        }
        
        task.resume()
        
    }
    
	func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber){
	
        if BLE_MODE == "non-beacon-mode" {
            
            /********** Parsing the BLE Advertising Packet ***********/
            /********** Find testing receive program in BLE_Scanner_iOS-TEST/ ***************/
            if let data_str = advertisementData["kCBAdvDataLocalName"]  {
                
                if String(describing: data_str) == "WLABpips" {
                    
                    ///////////////   print the timestamp with milliseconds granularity
                    endTime = NSDate().timeIntervalSinceReferenceDate
                    print("**************** time since last discovery ***************")
                    print("\((endTime-startTime) * 1000) millisec")
                    startTime = endTime
                    
                    ////////////// Parse the kCBAdvDataLocalName
                    var data_hexstr = [String]()
                    for codeUnit in ((data_str as? String)?.utf16)! {
                        
                        let temp_str = String(format: "%02X", Int(codeUnit))
                        data_hexstr.append(temp_str)
                    }
                    var data_local_name_string = ""
                    for element in data_hexstr { data_local_name_string += element}
                    print("data_local_name_string: \(data_local_name_string)")
                    
                    
                    ////////////// Parse the kCBAdvDataServiceUUIDs
                    let data_uuid = String(describing:  advertisementData["kCBAdvDataServiceUUIDs"]!)
                    var data_uuid_array_temp = [String]()
                    var flag = 0
                    print("length \(data_uuid.characters.count)")
                    for character in data_uuid.characters {
                        if(character == "\"") {
                            if(flag == 0) {flag = 1}
                            else {break}
                        }
                        else {
                            if (flag == 1 && character != "-") {
                                data_uuid_array_temp.append(String(character))
                            }
                        }
                    }
                    var data_uuid_string = ""
                    for i in 0...15 {
                        data_uuid_string += data_uuid_array_temp[31-(2*i)-1]
                        data_uuid_string += data_uuid_array_temp[31-(2*i)]
                        
                    }
                    print("data_uuid_string \(data_uuid_string)")
                    
                    
                    //////////////// Create final 31 byte packet
                    let ble_adv_packet_31_byte = "02011A1107" + data_uuid_string + "0909" + data_local_name_string
                    print("ble_adv_packet_31_byte: \(ble_adv_packet_31_byte)")
                    print("ble_adv_packet_31_byte length: \(ble_adv_packet_31_byte.characters.count)")
                    
                    /////////////// Parse the Node ID
                    let start = ble_adv_packet_31_byte.index(ble_adv_packet_31_byte.startIndex, offsetBy: 36)
                    let end = ble_adv_packet_31_byte.index(ble_adv_packet_31_byte.endIndex, offsetBy: -22)
                    let range = start..<end
                    let node_id_string = ble_adv_packet_31_byte.substring(with: range)
                    print("node_id_string: \(node_id_string)")
                    
                    //////////////// RSSI
                    print("RSSI: \(RSSI)")
                    
                    ///////////////  Send ble_adv_packet_31_byte and RSSI to local positioning algorithm
                    
                    
                    //////////////// Determine WHETHER to create new entry for newly discovered BLE node or to edit existing entry for existing BLE node
                    
                    // Update existing table entry for existing BLE node
                    for (index, foundPeripheral) in peripherals.enumerated(){
                        
                        if(foundPeripheral.node_id == node_id_string){
                            peripherals[index].lastRSSI = RSSI
                            tableView.reloadData()
                            return
                        }
                    }
                    
                    // Create new table entry for newly discovered BLE node
                    let displayPeripheral = DisplayPeripheral(node_id: node_id_string, lastRSSI: RSSI)
                    peripherals.append(displayPeripheral)
                    tableView.reloadData()
                }
            }
            else {
                //print("\n\nadvertisement data[\"kCBAdvDataLocalName\"] is nil")
            }
        }
        
        
        else if BLE_MODE == "beacon-mode" {
            
            /********** Parsing the BLE Advertising Packet ***********/
            /********** Find testing receive program in BLE_Scanner_iOS-TEST/ ***************/
            if let data_str = advertisementData["kCBAdvDataManufacturerData"]  {
                
                ////////////// Parse the iBeacon value
                var data_ibeacon = String(describing:  advertisementData["kCBAdvDataManufacturerData"]!)
               
                if data_ibeacon.characters.count > 40 {
                    
                    // remove "<", ">", " " in ibeacon packet string
                    data_ibeacon = data_ibeacon.components(separatedBy: ["<", " ", ">"]).joined()
                    
                    // Check for "WLAB"
                    var start = data_ibeacon.index(data_ibeacon.startIndex, offsetBy: 8)
                    var end = data_ibeacon.index(data_ibeacon.endIndex, offsetBy: -34)
                    var range = start..<end
                    if data_ibeacon.substring(with: range) == "574c4142" {
                        
                        ///////////////   print the timestamp with milliseconds granularity
                        endTime = NSDate().timeIntervalSinceReferenceDate
                        print("\n\n**************** time since last discovery ***************")
                        print("\((endTime-startTime) * 1000) millisec")
                        print("endtime: \(endTime)")
                        let elapsed = (endTime-startTime) * 1000
                        startTime = endTime
                        
                        if elapsed > 300.0 {
                            
                            print("ELAPSED TIME OF \(elapsed) HAS SLOWED DOWN.")
                            print("RESTART BLE SCAN")
                            centralManager?.stopScan()
                            startScanning()
                        }
                        
                        
                        
                        print(data_ibeacon)
                        print("data_ibeacon legnth: \(data_ibeacon.characters.count)")
                        
                        // parse Node ID
                        start = data_ibeacon.index(data_ibeacon.startIndex, offsetBy: 40)
                        end = data_ibeacon.index(data_ibeacon.endIndex, offsetBy: -5)
                        range = start..<end
                        let node_id_string = data_ibeacon.substring(with: range)
                        print("node_id: \(node_id_string)")
                        
                        //////////////// RSSI
                        print("RSSI: \(RSSI)")
                        
                        ///////////////  Send data_ibeacon and RSSI to local positioning algorithm
                        print(Libpipsapi.Test(1))
                        
                        var api_out = Libpipsapi.pips_api_out()
                        var cmt_msg = ""

                        if Libpipsapi.PIPS_rx_adv_data(data_ibeacon, Int32(data_ibeacon.characters.count), Int32(RSSI), elapsed, "", &api_out) == 1 {
                           
                            if api_out.new_pos_ready == 1 {
                                
                                let pos_coord_str = String(format: "(%5.2f, %5.2f, %5.2f) m", api_out.x, api_out.y, api_out.z)
                                postion_coordinate.text = pos_coord_str
                                
                                position_name.text = withUnsafePointer(to: &api_out.pos_name) {
                                    $0.withMemoryRebound(to: UInt8.self, capacity: MemoryLayout.size(ofValue: api_out.pos_name)) {
                                        String(cString: $0)
                                    }
                                }
                                rssi_node_1.text = String(format: "%5.1f", api_out.rssi.0)
                                rssi_node_2.text = String(format: "%5.1f", api_out.rssi.1)
                                rssi_node_3.text = String(format: "%5.1f", api_out.rssi.2)
                                rssi_node_4.text = String(format: "%5.1f", api_out.rssi.3)


                            }
                            
                            if api_out.new_msg_ready == 1 {
                                
                                debug_message.text = withUnsafePointer(to: &api_out.dbg_msg_out) {
                                    $0.withMemoryRebound(to: UInt8.self, capacity: MemoryLayout.size(ofValue: api_out.dbg_msg_out)) {
                                        String(cString: $0)
                                    }
                                }
                            }
                            
                            if api_out.new_comments_ready == 1 {
                                
                                cmt_msg = withUnsafePointer(to: &api_out.cmt_msg) {
                                    $0.withMemoryRebound(to: UInt8.self, capacity: MemoryLayout.size(ofValue: api_out.cmt_msg)) {
                                        String(cString: $0)
                                    }
                                }
                                api_out.new_comments_ready = 0
                            }
                        }

                        ///////////////  Send data to local file
                        //http_post("http://192.168.1.162:3000/api/Recordings", "elapsed=\(elapsed)&adv_len=\(Int32(data_ibeacon.characters.count))&RSSI=\(Int32(RSSI))&adv_data=\(data_ibeacon)&comment=\(cmt_msg)")
                        
                        let str = String(format: "%3.2f\t\t\(Int32(RSSI))\t\t\(Int32(data_ibeacon.characters.count))\t\t\(data_ibeacon)\t\t\(cmt_msg)\n", elapsed)
                        
                        File("WRITE", str)
                        
                        //////////////// Determine WHETHER to create new entry for newly discovered BLE node or to edit existing entry for existing BLE node
                        // Update existing table entry for existing BLE node
                        for (index, foundPeripheral) in peripherals.enumerated(){
                            
                            if(foundPeripheral.node_id == node_id_string){
                                peripherals[index].lastRSSI = RSSI
                                tableView.reloadData()
                                
                                // update rssi map 
                                /*
                                if node_id_string == "0100a" {
                                    rssi_node_1.text = "\(RSSI)"
                                }
                                else if node_id_string == "0200a" {
                                    rssi_node_2.text = "\(RSSI)"
                                }
                                else if node_id_string == "0302a" {
                                    rssi_node_3.text = "\(RSSI)"
                                }
                                else if node_id_string == "0402a" {
                                    rssi_node_4.text = "\(RSSI)"
                                }
                                */
                                
                                return
                            }
                        }
                        
                        // Create new table entry for newly discovered BLE node
                        let displayPeripheral = DisplayPeripheral(node_id: node_id_string, lastRSSI: RSSI)
                        peripherals.append(displayPeripheral)
                        tableView.reloadData()
                    }
                }
            }
            else {
                //print("\n\nadvertisement data[\"kCBAdvDataLocalName\"] is nil")
            }
        }
	}
}

extension PeripheralViewController: UITableViewDataSource {
	func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell{
		
		let cell = self.tableView.dequeueReusableCell(withIdentifier: "cell")! as! DeviceTableViewCell
		cell.displayPeripheral = peripherals[(indexPath as NSIndexPath).row]
		return cell
	}
	
	func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int{
		return peripherals.count
	}
}




