import UIKit
import CocoaAsyncSocket

class ViewController: UIViewController, GCDAsyncUdpSocketDelegate {
    let host_address = "10.11.110.40"
    let host_port: UInt16 = 25592
    let client_port: UInt16 = 22234


    var udpSocket: GCDAsyncUdpSocket!
    var peerAddress: String?
    var peerPort: UInt16?

    @IBOutlet weak var messageField: UITextField!
    @IBOutlet weak var labelMess: UILabel!

    override func viewDidLoad() {
        super.viewDidLoad()
        setupNetworkCommunication()
    }

    func setupNetworkCommunication() {
        udpSocket = GCDAsyncUdpSocket(delegate: self, delegateQueue: DispatchQueue.main)

        do {
            try udpSocket.bind(toPort: client_port)
            try udpSocket.beginReceiving()
            sendRequestAddress()
        } catch {
            print("Error setting up UDP connection: \(error.localizedDescription)")
        }
    }

    func sendRequestAddress() {
        let requestMessage = "RequestAddress"
        let data = requestMessage.data(using: .utf8)!
        udpSocket.send(data, toHost: host_address, port: host_port, withTimeout: -1, tag: 0)
    }

    func udpSocket(_ sock: GCDAsyncUdpSocket, didConnectToAddress address: Data) {
        print("Connected to address: \(address)")
        sendRequestAddress()
    }

    func udpSocket(_ sock: GCDAsyncUdpSocket, didNotConnect error: Error?) {
        print("Failed to connect: \(error?.localizedDescription ?? "")")
    }

    func udpSocket(_ sock: GCDAsyncUdpSocket, didSendDataWithTag tag: Int) {
        print("Message sent")
    }

    @IBAction func sendBtn(_ sender: Any) {
        if let message = messageField.text, !message.isEmpty {
            if let peerAddress = peerAddress, let peerPort = peerPort {
                let messageData = message.data(using: .utf8)!
                udpSocket.send(messageData, toHost: peerAddress, port: peerPort, withTimeout: -1, tag: 0)
                
                // 보낸 메시지를 표시하는 레이블을 생성하고 추가합니다.
                let sentLabel = UILabel()
                sentLabel.text = "Sent message: \(message)"
                sentLabel.numberOfLines = 0
                sentLabel.frame = CGRect(x: 20, y: 20 + (30 * (labelMess.subviews.count)), width: 300, height: 30)
                labelMess.addSubview(sentLabel)
                
                messageField.text = "" // 텍스트 필드 초기화
            } else {
                print("Peer address not available")
            }
        }
    }

    func udpSocket(_ sock: GCDAsyncUdpSocket, didReceive data: Data, fromAddress address: Data, withFilterContext filterContext: Any?) {
        if let message = String(data: data, encoding: .utf8) {
            print("Received message: \(message)")
            if message.hasPrefix("PeerAddress:") {
                let components = message.components(separatedBy: ":")
                peerAddress = components[1]
                peerPort = UInt16(components[2])
            } else {
                // 받은 메시지를 표시하는 레이블을 생성하고 추가합니다.
                let receivedLabel = UILabel()
                receivedLabel.text = "Received message: \(message)"
                receivedLabel.numberOfLines = 0
                receivedLabel.frame = CGRect(x: 20, y: 20 + (30 * (labelMess.subviews.count)), width: 300, height: 30)
                labelMess.addSubview(receivedLabel)
            }
        } else {
            print("Received invalid message")
        }
    }
}
