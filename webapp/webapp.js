var myApp= angular.module('myApp', [
    'ngRoute',
    'mobile-angular-ui',
	'btford.socket-io'
]);

myApp.config(function($routeProvider) {
    $routeProvider.when('/', {
        templateUrl: 'home.html',
        controller: 'Home'
    });
});

myApp.factory('mySocket', function (socketFactory) {
	var myIoSocket = io.connect('/webapp');	//Tên namespace webapp

	mySocket = socketFactory({
		ioSocket: myIoSocket
	});
	return mySocket;
	
/////////////////////// Những dòng code ở trên phần này là phần cài đặt, các bạn hãy đọc thêm về angularjs để hiểu, cái này không nhảy cóc được nha!
});

myApp.controller('Home', function($scope, mySocket) {
	////Khu 1 -- Khu cài đặt tham số 
    //cài đặt một số tham số test chơi
	//dùng để đặt các giá trị mặc định
    $scope.humidity = "Chưa kết nối";
    $scope.temperature = "Chưa kết nối";
    $scope.CamBienDoAm = "Chưa kết nối";
    $scope.doAmAnalog = "Chưa kết nối";
    $scope.camBienAnhSang = "Chưa kết nối";
    $scope.slider = {value: 100};
    $scope.delays_status = [0, 0]
	
	////Khu 2 -- Cài đặt các sự kiện khi tương tác với người dùng
	//các sự kiện ng-click, nhấn nút
	$scope.updateSensor  = function() {
		mySocket.emit("doAm")
		mySocket.emit("DHT11")		
		mySocket.emit("DELAY")
	}
	
	$scope.changeDelay = function() {
		console.log("send status ", $scope.delays_status)
		
		var json = {
			"delay": $scope.delays_status
		}
		mySocket.emit("DELAY", json)
	}
	
	////Khu 3 -- Nhận dữ liệu từ Arduno gửi lên (thông qua ESP8266 rồi socket server truyền tải!)
	//các sự kiện từ Arduino gửi lên (thông qua esp8266, thông qua server)
	mySocket.on('DHT11', function(json) {
		$scope.humidity =  json.hum + "%"
		$scope.temperature = json.tem + "oC"
		$scope.camBienAnhSang= json.camBienAnhSang+" lux"			
	})

	mySocket.on('doAm', function(json) {
		$scope.doAmAnalog =  json.analog +"%"
		$scope.CamBienDoAm = (json.digital == 1) ? "Cần tưới nước!" : "Đủ nước" 
	})
	//Khi nhận được lệnh LED_STATUS
	mySocket.on('DELAY_STATUS', function(json) {
		//Nhận được thì in ra thôi hihi.
		console.log("recv DELAY", json)
		$scope.delays_status = json.data
		$scope.slider=json.slider
	})
	
	
	//// Khu 4 -- Những dòng code sẽ được thực thi khi kết nối với Arduino (thông qua socket server)
	mySocket.on('connect', function() {
		console.log("connected")
		mySocket.emit("doAm")
		mySocket.emit("DHT11")
		mySocket.emit("DELAY")
	})
		
});