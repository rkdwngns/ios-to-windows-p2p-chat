# ios-to-windows-p2p-chat
이 프로그램은 ios와 windows와의 채팅을 하는 프로그램으로
네트워크의 구조는 하이브리드p2p방식을 사용하였고 클라이언트는 2가지 
서버는 1가지로 구현하였습니다

'''swift
    @IBAction func sendBtn(_ sender: Any) {
        if let message = messageField.text, !message.isEmpty, let peerAddress = peerAddress {
            sendMessageToPeer(message: message, peerAddress: peerAddress)
        }
    }
'''
