# Table of Content

[Project Background:](#project-background)
  - [Project Devices:](#project-devices)
  - [Project Preparation:](#project-preparation)
[Project Overview](#project-overview)
  - [Topology](#topology)
      - [Alexa skill](#alexa-skill)
      - [AWS IoT Core](#aws-iot-core)
      - [AWS FreeRTOS:](#aws-freertos)
      - [Bluetooth mesh network:](#bluetooth-mesh-network)
  - [Technical details:](#technical-details)
      - [Alexa Smart Home Skill](#alexa-smart-home-skill)
      - [AWS IoT Core](#aws-iot-core-1)
      - [ESP32 and AWS freeRTOS](#esp32-and-aws-freertos)
      - [Bluetooth Mesh Network](#bluetooth-mesh-network)
      - [Communications:](#communications)
[Replication the project](#replication-the-project)
  - [Preparation](#preparation)
      - [Components:](#components)
      - [Software Used:](#software-used)
  - [Procedures:](#procedures)
  - [1.	Configure the AWS IoT core settings:](#1-configure-the-aws-iot-core-settings)
  - [Download and set up freeRTOS SDK:](#download-and-set-up-freertos-sdk)
      - [Download the project](#download-the-project)
      - [Build the esp32 program](#build-the-esp32-program)
      - [Build BG13 embedded provisioner program](#build-bg13-embedded-provisioner-program)
      - [Build MG21 Bluetooth mesh light/switch/sensor/sensor monitor program](#build-mg21-bluetooth-mesh-lightswitchsensorsensor-monitor-program)
      - [Lambda function:](#lambda-function)
      - [Alexa skill:](#alexa-skill)
[Conclusion:](#conclusion)


# Project Background:
As the development of the smart home market, the need for controlling smart devices using human voice grows rapidly. The leading companies all over the world introduced many different solutions in the area of smart home to satisfy their customers’ requirements. Among those solutions, the most popular pattern is to control smart devices via a central smart speaker, which listens to the user’s voice command and responds with actions, such as turning on/off smart lights, adjusting the room temperature through sending remote signals to the AC, and even cooking a nice meal by operating an intelligent robot. 
However, one thing often happens that breaks this good image is that the customers often find the smart speaker does not support their smart devices. That happens mainly due to one big reason—the communicating protocols used by the smart speaker and the endpoint device are not same.  
Currently, there are three protocols mostly used in the smart home industry: Bluetooth mesh, Zigbee, and Z-Wave. In Chinese and the U.S. market, Bluetooth mesh and Zigbee are the mainly used protocols due to their many advantages, such as low power consumption, extendible network size, and strong security.  
In the US, the Zigbee is selected by Amazon as the communicating protocol between its smart speaker brand —Amazon Echo -- and the various smart devices in its ecosystem.  
While in China, the Bluetooth mesh is the primary protocol that most smart devices company are using. Therefore, the Amazon Echo is not able to control those devices due to the incompatible communicating protocol.  
In this project, a solution is proposed to solve this problem.  
A Bluetooth mesh network is going to be controlled by an Amazon Echo Plus 2 via cloud-to-cloud approach. The user can turn on and turn off a Bluetooth mesh light by giving a voice command to the Echo Plus.  

## Project Devices:
* Amazon Echo Plus 2  
* Silicon Labs Wireless Gecko EFR32xG21 *2  
* Silicon Labs Blue Gecko EFR32BG13  
* Espressif Esp32 WROOM 32D	 

## Project Preparation:
* Install the Alexa App in you iOS device. Note that the Alexa App is only available for some special regions, for e,g. US, you need to Login the AppStore with a special Apple ID. And below is an Apple ID hosted by APAC regional apps team, you can feel free to use it.
   * UserName: silabs_iot_ra_apac@outlook.com  
   * Password: Register_2019  
* Register the Amazon with your own account or use the available account below. For this demonstration, we have registered the account as below.
   * UserName: silabs_iot_ra_apac@outlook.com  
   * Password: register_2019  
* Also you can / should Sign-in the Alexa App with the same account as above.
* If Amazon requests for account verification with email, please sign in the outlook with the account below.
   * UserName: silabs_iot_ra_apac@outlook.com  
   * Password: register_2019  

* Register AWS in https://portal.aws.amazon.com/billing/signup#/start. The AWS is used for holding the lambda function, and credit card information is needed for registering a new account of the AWS.
   * The account information for AWS is not public.
   * UserName: yuancheng@xxxxxx
   * Password: xxxxxx
 
# Project Overview
## Topology
 
<div align="center">
  <img src="./images/topology_block_diagram.png">
</div>

### Amazon Echo Plus 2
Amazon Echo is a brand of smart speakers developed by Amazon. Echo devices connect to the voice-controlled intelligent personal assistant service Alexa, and the Echo Plus has a built-in Zigbee hub to easily setup and control your compatible smart home devices.

### AWS Lambda
AWS Lambda lets you run code without provisioning or managing servers. You pay only for the compute time you consume - there is no charge when your code is not running.
With Lambda, you can run code for virtually any type of application or backend service - all with zero administration. Just upload your code and Lambda takes care of everything required to run and scale your code with high availability. You can set up your code to automatically trigger from other AWS services or call it directly from any web or mobile app.
<div align="center">
  <img src="./images/diagram_Lambda-HowItWorks.png">
</div>

### Alexa skill
The Alexa skill is the bridge that connects the users to AWS IoT could.  
After Amazon Echo receiving the user’s voice commands, it will sent the voice record to AWS Alexa server(AI assistant server), where interprets the voice command to control directives. Then, the Alexa server sends the directives to lambda server, **a place running the skill code**. In the Lambda server, the specific action is executed according to the content of the directives. For example, if the user says “Alexa, turn on my light”, the Alexa will analysis this voice command and sending a JSON formatted file which listed a bunch of information about the light and actions to be executed to lambda, and then lambda will modify the light status from “OFF” to “ON” in the database.

### AWS IoT Core 
AWS IoT Core is a powerful platform to manage IoT devices remotely. It has a service called **Thing Shadow**, which is a JSON document recording the information of the devices it manages. In this project, the AWS IoT plays the role of the system database. It records every device’s status in the Thing Shadow document and could be visited by ESP32 via the integrated AWS freeRTOS SDK.
**Thing Shadows**: a JSON document that is used to store and retrieve current state information.

### AWS FreeRTOS  
AWS freeRTOS is running on ESP32 board. It connects the local network to the AWS cloud.  
ESP32: ESP32 is the gateway in this project. When it runs, it continuously gets the Thing Shadow document from the AWS IoT Core, and then it translates the JSON directives to simple character strings. Through UART communication, it passes the commands to the provisioner in the Bluetooth Mesh network, and the provisioner will give the specific directives to appointed devices.

### Bluetooth mesh network:
In this project, a simple Bluetooth network is created that includes two mesh node and of course a provisioner.  
The provisioner in the network to provision new devices and receive commands from ESP32 via UART, and also parse the commands from ESP32 and transfer to the mesh command and transmite it to the mesh network for controlling other nodes.  
There are total two Bluetooth mesh nodes in the network, a light node and a switch node. Once the user presses the button on the switch node, the light node would receive a BLE mesh message and turn on/off the embedded LED on it.  

<div align="center">
  <img src="./images/wireless_gecko_btmesh_light_node.jpg">
</div>

<div align="center">
  <img src="./images/wireless_gecko_btmesh_switch_node.jpg">
</div>

Also, the switch node is capable to change the online shadow document. When the user give it a long press on left button, the switch node will send a message to provisioner to update the online shadow document.

<div align="center">
  <img src="./images/local_message_flow.png">
</div>

## Technical details:
In this section, the technical details below will be introduced.
* Alexa Smart Home Skill
* ESP32 freeRTOS application
* Bluetooth Mesh provisioner

sample codes, including lambda function, Alexa smart home skill, ESP32 freeRTOS application, and the codes of the Bluetooth Mesh provisioner (based on EFR32BG13) will also be explained.  

However, because of the limited length of this article, if the reader wants to replicate this project, please read the section of [Replication the project](#replication-the-project) for the step by step guidance, and access the GitHub page: https://github.com/sheldon123z and find the mentioned packages accordingly.

### Alexa Smart Home Skill
Alexa smart home skill interface is designed for controlling smart home devices using Amazon Echo series smart speaker by Amazon. In this project, an Echo plus 2 was utilized to receive the voice command and transmit the voice command to the Alexa server.
The skill is held by a lambda function, which means the code is running on the Amazon lambda server. When a user gives the smart speaker a voice command, the command will be firstly analyzed on Alexa server. Then, a JSON format directive will be sent to lambda. There are many different directives. The most important one is the discovery directive, which indicates the speaker to find any available devices and report back to the Alexa server.
In this project, the response is also generated by the same lambda function. After receiving the discovery directive, the lambda function will directly send a response message back. Therefore, the device information needs to be programmed in the codes.

```
if namespace == 'Alexa.Discovery':  
    if name == 'Discover':  
        adr = AlexaResponse(namespace='Alexa.Discovery', name='Discover.Response')  
        #create capability part for the response  
        adr = create_discover_response(adr)               
        return send_response(adr.get())   
```

The “```AlexaResponse()```” function is used to generate the response — a JSON document. It uses **kwags parameter to receive whatever the user appointed attributes. Among those attributes, the “namespace” and “name” is necessary to add. Alexa server will use this information to identify what the response is and give a feedback to the user.

```
#create the discover response   
#first create the capabilities that the endpoints need   
#then add the endpoints to the response entity, and add the capability information to the endpoints  
def create_discover_response(response):  
    #general response  
    capability_alexa = response.create_payload_endpoint_capability()  
    #specific capabilities  
    #power controller--turn on turn off operations  
    capability_alexa_PowerController = response.create_payload_endpoint_capability(  
        interface='Alexa.PowerController',  
        supported=[{'name': 'powerState'}])  
    #create colorcontroller capability  
    capability_alexa_ColorController = response.create_payload_endpoint_capability(  
        interface='Alexa.ColorController',  
        supported=[{'name': 'color'}])  
    #create brightnessController capability  
    capability_alexa_BrightnessController = response.create_payload_endpoint_capability(  
        interface='Alexa.BrightnessController',  
        supported=[{'name': 'brightness'}])  
    #create colortemperature capability  
    capability_alexa_ColorTemperatureController = response.create_payload_endpoint_capability(  
        interface='Alexa.ColorTemperatureController',  
        supported=[{'name': 'colorTemperatureInKelvin'}])  
    #create lock controller capability  
    capability_alexa_lockcontroller = response.create_payload_endpoint_capability(  
        interface='Alexa.LockController',  
        supported=[{'name':'lockState'}]  
    )  
    capability_alexa_endpointHealth = response.create_payload_endpoint_capability(  
        interface='Alexa.EndpointHealth',  
        supported = [{'name':'connectivity'}]  
    )       
```

The ```capability``` and ```endpoint``` id compose the virtual representation of a smart device. One device can simultaneously own several different capabilities. For example, a light can support PowerController, ColorController, and ColorTemperatureController capabilities to control the on/off, color, and temperature attributes.
After constructing the capabilities of the device, we also need to add other device information and the endpoint id to the response.

```
response.add_payload_endpoint(  
    friendly_name='Sample Switch',  
    endpoint_id='sample-switch-01',  
    manufactureName = 'silicon labs',  
    display_categories = ["SWITCH"],  
    discription = 'silicon labs product',   
    capabilities=[capability_alexa, capability_alexa_PowerController])  
```

The "```display_categories```" item will tell Alexa what type of the device is. Alexa will show the corresponding icon on the Alexa phone application.
The discovery response is needed only once. It is similar to the process of registering the device to Alexa server. After this registration, Alexa is connected to the smart devices; the user can see the device on the Alexa application.  
Every time Alexa gives a directive, the lambda function will respond a corresponding message to Alexa and execute the directive via changing the status of the device on the IoT core. In the lambda function, every controller interface needs a specific method to handle.  

```
def respond_brightnessControl_dir(request):  
    # Note: This sample always returns a success response for either a request to TurnOff or TurnOn  
    endpoint_id = request['directive']['endpoint']['endpointId']  
    correlation_token = request['directive']['header']['correlationToken']  
    token = request['directive']['endpoint']['scope']['token']  
    name = request['directive']['header']['name']  
    #get the brightness from the directive  
    brightness_value = request['directive']['payload']['brightness']  
    thingshadow_updated = update_thing_shadow(thing_name_id=endpoint_id,brightness=brightness_value)  
      
    #check the operation if successful  
    if not (thingshadow_updated):  
        return AlexaResponse(  
        name='ErrorResponse',  
        payload={'type': 'ENDPOINT_UNREACHABLE', 'message': 'Unable to reach endpoint database.'}).get()  
    #abtr: alexa brightness response  
    abtr = AlexaResponse(  
        coorelation_token = correlation_token,  
        endpoint_id=endpoint_id,  
        token = token)      
    abtr.add_context_property(namespace = 'Alexa.BrightnessController',name = "brightness",value = brightness_value)  
```

For instance, a brightness controlling directive for light could be executed and responded by the function above. The lambda function will first analyze the directive, extract the information such as the ```endpoint_id```, the ```correlation_token``` (which is used in response to Alexa), and the specific actions to be done. Then the lambda function will access to the IoT core, update the corresponding attribute information on the appointed virtual device using ```update_thing_shadow``` function. If the updating action is done successfully, lambda will give an acknowledging response to Alexa; else it will give an ```ErrorResponse```.  

As mentioned above, the **Lambda function** is the place to hold **Alexa skill**. The codes in the lambda function are basically the essence of the Alexa skill. However, to associate a Lambda function and an Alexa smart home skill, several steps need to be done. Please see the section [Replication the project](#replication-the-project) for the detailed steps for replicating this project on your end.


Lambda function:
As introduced before, the **lambda function** is the place holding Alexa skills. Every “directive” is processed and handled in the lambda function. It is quite complex to set up a lambda function, the detailed steps to set up a lambda function can be found [here](https://docs.aws.amazon.com/lambda/latest/dg/getting-started.html), and the Chinese version documentation is available [here](https://docs.aws.amazon.com/zh_cn/lambda/latest/dg/getting-started.html).
The lambda function has multiple servers in different areas of the world. However, some of them don’t support Alexa smart home skills. This project chooses **US East(N.Virgina)** server to hold the skill.

<div align="center">
  <img src="./images/lambda_function_console.png">
  <center> <b>Figure: Lambda function console</b> </center>
</div>  

On the lambda function console, paste the Alexa smart home skill code to the editor. On the upper right corner, the testing case can be built. You can define different directives to test the skill if it works well or not. 

<div align="center">
  <img src="./images/turn_on_directive_test_case.png">
  <center> <b>Figure: A TurnOn directive test case</b> </center>
</div>  

The directive will be the input for the specified lambda handler. A quick click on the test button, the response will show up on the bottom of the console.

<div align="center">
  <img src="./images/response_of_the_lambda_function.png">
  <center> <b>Figure: Response of the lambda function</b> </center>
</div>  

It is worth to mention the rating of the lambda function. The AWS lambda has free tiers for each user, up to 1,000,000 free requests per month. In this project, only requests to lambda function and IoT Core are used, so the free tier is totally enough.

<div align="center">
  <img src="./images/aws_lambda_free_tiers.png">
  <center> <b>Figure: AWS lambda free tiers</b> </center>
</div>  


### AWS IoT Core
AWS IoT Core is another platform of AWS IoT service.  It stores the status information of the remote IoT devices in a special service called Thing Shadow. Briefly speaking, the thing shadow is a JSON document for recording the real-time changes of the device status.

<div align="center">
  <img src="./images/iot_core_thing_shadow.png">
  <center> <b>Figure: IoT Core Thing Shadow</b> </center>
</div>  

The Thing shadow can be modified by both lambda function and local mesh network. In this project, it has been set with four nested layers: state, desired/reported, device name, and attributes. 
The “state” and “desired/reported” is required for each document uploaded, and once the user uploads a new document, only the items in the new document will be updated. The rest of the items will remain the same value. Also, if the desired/reported sections are not the same value, the document will automatically generate a new section called delta, which records the differences between the desired section and reported section.

<div align="center">
  <img src="./images/jason_document_for_thing_shadow.png">
  <center> <b>Figure: JSON document for thing shadow</b> </center>
</div>  

### ESP32 and AWS freeRTOS
ESP32 is the network gateway and responsible for downloading the shadow document, uploading new shadow document, and forwarding directives to the local mesh network. It communicates with the provisioner of the mesh network via UART communication. The JSON document will be first converted to a local string directive and then sent to the provisioner board.  
The ESP32 board acquires shadow document from IoT core via MQTT protocol. On the IoT Core, each device has a virtual counterpart. To get the shadow document of a device, the user needs to publish a blank message to the MQTT topic “$aws/things/device name/shadow/get”.  The MQTT server will respond with the specified shadow document. Similar operations such as update the shadow and delete the shadow can also be done in this way.
The code in the ESP32 follows a linear structure. There are two ways to get the thing shadow from IoT console. For the first one, it begins with establishing internet connection and MQTT connection, then using the function “AwsIotShadow_Get” to acquire the shadow document continuously in an infinite loop. Once it obtained the shadow, it transfers the text into a more straightforward string directive. The provisioner will also give an acknowledging message with the same format after the directive has been executed. The esp32 will according to the message to update the thing shadow document in the next loop.
For the second one, a callback function called xxx (the information is missing, I need to repair it later.)

<div align="center">
  <img src="./images/esp32.jpg">
  <center> <b>Figure: ESP32</b> </center>
</div>  

### Bluetooth Mesh Network
The Bluetooth mesh network is composed of three parts: A switch node, a light node, and a provisioner node.
The provisioner in the network is responsible for establishing the mesh network, authorizing new devices, and receiving messages from outside. The provisioner program obeys the “event-driven” pattern in a big “switch” structure. Every coming event will have an event id and the even data. The application invokes different handler functions to extract information and give responses back according to the event id. Through using the UART interrupt, the provisioner can detect messages come from the esp32 board. Also, when the provisioner receives the event id, which indicates the status changes from mesh network endpoints, it will invoke the UART TX handler to send messages to the ESP32. Moreover, the switch node can control the light node. The provisioner also subscribed the switch node message, which means that it will receive a notification when the switch turns on or turns off the light. The status information of the node will be displayed directly on the Alexa phone application, and the user can also control the devices via the app.

### Communications:
This project uses different protocols to do communications among different parts. The MQTT protocol is utilized for communicating with the **AWS IoT Core** console, where the thing shadow document is stored; and the Bluetooth mesh is used to organize local devices. Between the ESP32 and the provisioner node in the mesh network, a self-defined simple UART protocol is also used to transmit the devices information and attribute information. Each packet of the UART protocol contains 41 char bytes, and the format is shown below.

<div align="center">
  <img src="./images/uart_packet_format.png">
  <center> <b>Figure: UART packet format</b> </center>
</div>  

The first byte is used to identify what type of operation is. Currently, the operation supports only change device state; the following bytes are used to determine the device type, the attribute name, and the attribute value. What is worth to pay attention is that the attribute value possibly number or string, therefore, in the code it has to be adjusted using the function “atoi()”

<div align="center">
  <img src="./images/operation_type_definition.png">
  <center> <b>Figure: Operation type definition</b> </center>
</div>  

<div align="center">
  <img src="./images/device_type_definition.png">
  <center> <b>Figure: Device type definition</b> </center>
</div>  

<div align="center">
  <img src="./images/attribute_type_definition.png">
  <center> <b>Figure: Attribute type definition</b> </center>
</div>  

<div align="center">
  <img src="./images/light_default_data_definition.png">
  <center> <b>Figure: Light default data definition</b> </center>
</div>  

# Replication the project
In this section, a step-by-step instruction will be provided to the reader to replicate the project.
## Preparation
### Components: 
* 1. WSTK board x3 (if possible, 5 is the best)
* 2. EFR32BG13 2.4GHz 10dBm x1  
* 3. EFR32 xG21 2.4GHz 10dBm x2  
* 4. Espressief ESP32-DevKitC or ESP-WROVER-KIT x1  
* 5. Wire jumpers x3  
* 6. CP2103-EB(for debugging)  
### Software Used:  
* 1. Simplicity Studio IDE  
* 2. Bluetooth mesh SDK v1.5.0 or above  
* 3. Amazon AWS freeRTOS SDK (click to open github page)  
* 4. Any COM port monitor tool (Tera Term)  

## Procedures:
The picture below illustrates the block diagram of controlling Physical Device (that is Bluetooth Mesh devices in this project) via Amazon Echo. We will split the procedures as several parts below.
<div align="center">
  <img src="./images/replicate_step0.png">
</div>  
</br>

* [Configure the AWS IoT core settings](#configure-the-aws-iot-core-settings)
* [Create Alexa Skill](#create-alexa-skill)
* [Lambda function](#lambda-function)
* [Download and set up freeRTOS SDK on ESP32](#download-and-set-up-freertos-sdk-on-esp32)

###	Configure the AWS IoT core settings: 
<div align="center">
  <img src="./images/replicate_step1.png">
</div>  
</br>

1. Setting up your AWS Account and Permissions [EN](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-account-and-permissions.html) | [CN](https://docs.aws.amazon.com/zh_cn/freertos/latest/userguide/freertos-account-and-permissions.html), this step gives you right to access AWS IoT console
2. Log into [AWS IoT console](https://us-east-1.console.aws.amazon.com/iot/home?region=us-east-1#/dashboard), open the services and choose **N.Virginia** as the server. Because some of the necessary features (IoT Core, Amazon FreeRTOS, etc.) are only supported on this server, so it's recommended to choose this server, and continue the following steps.

Navigate to the **IoT Core Service**, below is the screenshot if you log into the AWS IoT console successfully.

<div align="center">
  <img src="./images/login_aws_iot_console.gif">
  <center> <b>Figure: AWS IoT console</b> </center>
</div>  

3. Create a thing called “esp32_btmesh_bridge” on your AWS console: 
click Manage -> Thing -> Register a thing -> Create -> Create a Single thing -> Create certificate -> Download the certificate and Activate. 
**Note**: Carefully preserve the thing certificates downloaded. 

<div align="center">
  <img src="./images/create_a_thing.gif">
  <center> <b>Figure: Create a thing</b> </center>
</div>  
</br>

4.	Go back to the console, open your thing “esp32_btmesh_bridge”. Click Security -> Your certificate -> Policies -> Actions -> Attach Policy. If you have a policy, change it to. If not, please refer to next step and then back to here.
<div align="center">
  <img src="./images/attach_policy_to_ting.gif">
  <center> <b>Figure: Attach Policy</b> </center>
</div>  
</br>

5.	Please don’t forget to modify the “Resource” of the policy as the lambda of yours.

```
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "iot:Connect",
      "Resource": "arn:aws:iot:us-east-1:879362360571:*"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Publish",
      "Resource": "arn:aws:iot:us-east-1:879362360571:*"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Subscribe",
      "Resource": "arn:aws:iot:us-east-1:879362360571:*"
    },
    {
      "Effect": "Allow",
      "Action": "iot:Receive",
      "Resource": "arn:aws:iot:us-east-1:879362360571:*"
    }
  ]
}
```
If you don’t have a policy, go back to console -> secure -> policy -> create You can create a policy to then attach it.

<div align="center">
  <img src="./images/create_a_policy.gif">
  <center><b>Figure: Create a policy in this interface</b></center>
</div>  
</br>

After creating the policy, the UI will similar like below.

<div align="center">
  <img src="./images/create_a_policy_result.png">
</div>  
</br>

6.	Click on the Manage -> Thing -> esp32_btmesh_bridge -> Shadow -> Edit, paste the initial shadow document below. The initial shadow document must have the same desired and reported section, if you want to change the shadow document, remember to change the shadow document template on the esp32 board and Alexa skill, otherwise the program won’t run correctly.

<div align="center">
  <img src="./images/shadow_document.gif">
  <center><b>Figure: Shadow document</b></center>
</div>  
</br>

Below is the initial Shadow document utilized in this project.
```
{
  "desired": {
    "Lights": {
      "device info": "default information",
      "brightness": 55,
      "value": {
        "value": 0,
        "hue": 0,
        "saturation": 1,
        "brightness": 1
      },
      "colorTemperatureInKelvin": 3000,
      "ON_OFF": "OFF",
      "LightID": "sample-light"
    },
    "Switch": {
      "Switch value": "OFF"
    },
    "Lock": {
      "Lock value": "LOCKED"
    },
    "Thing name": "sample-switch-01",
    "color": "RED",
    "sequence": [
      "RED",
      "GREEN",
      "BLUE"
    ]
  },
  "reported": {
    "Lights": {
      "device info": "default information",
      "brightness": 55,
      "value": {
        "value": 0,
        "hue": 0,
        "saturation": 1,
        "brightness": 1
      },
      "colorTemperatureInKelvin": 3000,
      "ON_OFF": "OFF",
      "LightID": "sample-light"
    },
    "Switch": {
      "Switch value": "OFF"
    },
    "Lock": {
      "Lock value": "LOCKED"
    },
    "Thing name": "sample-light",
    "color": "GREEN"
  },
  "delta": {
    "Thing name": "sample-switch-01",
    "color": "RED",
    "sequence": [
      "RED",
      "GREEN",
      "BLUE"
    ]
  }
}
```

7.	Use the test function on AWS IoT console to check if the rule that using the MQTT works or not.
Click on the “Interact” option, choose the corresponding topic which represents the operations that you wish to do, subscribe to the topic. Below are the all available MQTT topics for subscribe or publish. For example, you can publish the message to the topic ```$aws/things/esp32/shadow/update``` to update the thing shadow, and subscribe to the MQTT topics ```$aws/things/esp32/shadow/update/accepted``` and ```$aws/things/esp32/shadow/update/documents``` for the accepted messages, also you can monitor the topic ```$aws/things/esp32/shadow/update/rejected``` for debugging purpose. If the message was rejected, you can get the error code by subscribing the ```$aws/things/esp32/shadow/update/rejected``` topic.
Receiving the MQTT message via the subscribing topic ```$aws/things/esp32/shadow/update/accepted``` means that the thing shadow works now.

<div align="center">
  <img src="./images/topic_to_subscribe.png">
  <center><b>Figure: Topic to subscribe</b></center>
</div>  
</br>  

<div align="center">  
  <img src="./images/subscribe_the_topic_and_publish_content_you_want.gif">  
  <center><b>Figure: Subscribe the topic and publish content you want</b></center>  
</div>  
</br>  

You can find an example shadow document [here](https://docs.aws.amazon.com/iot/latest/developerguide/using-device-shadows.html). When you send shadow update messages to the topic ```$aws/things/<shadow-name>/shadow/update```, please make sure it at least contains the **state** field.

```
{
  "state": {
    "desired": {
      "Lights": {
        "device info": "default information",
        "brightness": 85,
        "value": {
          "value": 0,
          "hue": 0,
          "saturation": 1,
          "brightness": 1
        },
        "colorTemperatureInKelvin": 3000,
        "ON_OFF": "ON",
        "LightID": "sample-light"
      },
      "Switch": {
        "Switch value": "OFF"
      },
      "Lock": {
        "Lock value": "LOCKED"
      },
      "Thing name": "sample-light"
    },
    "reported": {
      "Lights": {
        "device info": "default information",
        "brightness": 55,
        "value": {
          "value": 0,
          "hue": 0,
          "saturation": 1,
          "brightness": 1
        },
        "colorTemperatureInKelvin": 3000,
        "ON_OFF": "OFF",
        "LightID": "sample-light"
      },
      "Switch": {
        "Switch value": "OFF"
      },
      "Lock": {
        "Lock value": "LOCKED"
      },
      "Thing name": "sample-light"
    }
  }
}
```

###	Create Alexa Skill
1.	Log into the [Alexa developer console](https://developer.amazon.com/alexa/console/ask/create-new-skill) with your amazon account.
2.	Choose Create skill->Smart Home->Enter the skill name->Create. Once you created the Smart Home Skill, <span id = "YourSkillID"><font color="red">Your Skill ID</font></span> will be assigned that will used by Lambda function.

<div align="center">
  <img src="./images/create_a_new_skill.png">
</div>
</br>

<div align="center">
  <img src="./images/create_a_new_skill_2.png">
</div>
</br>

At the Alexa console, the “Default endpoint” should be set as the endpoint of your lambda function. Also, the skill ID should also be set on the lambda function. Therefore, the skill is associated with the lambda function.

<div align="center">
  <img src="./images/fill_default_endpoint_info.png">
</div>   
</br>

<div align="center">
  <img src="./images/lambda_function_adding_alexa_smart_home_trigger.png">
  <center> <b>Figure: Lambda function adding Alexa smart home trigger</b> </center>
</div>  

3.	After you created the skill, the first step is to setup the account linking. Smart Home skills require that a user completes account linking during skill enablement. This step asks customers to associate their device cloud account with your smart home skill. You will need an OAuth provider in order to implement this process. If you don't already have an OAuth provider, you can use Login with Amazon (LWA).  

**Setup the LWA**:  
* i. Connect to https://developer.amazon.com/login.html and authenticate with your Amazon credentials.
* ii. Click "Login with Amazon"
* iii. Click on “Create a New Security Profile”

<div align="center">
  <img src="./images/LWA_setup.png">
</div> 

* iv. Fill in all three required fields to create your security profile and click “Save”. For the “Consent Privacy Notice URL”, please fill it with your own privacy notice URL.

<div align="center">
  <img src="./images/security_profile_management.png">
</div> 

* v. Before you complete this step, be sure to click on the link named “Show Client ID and Client Secret” and save these values to a secure location so they're easily available later. You’ll need these values later in a future step.  

5.	Configure the skill:
* i. Go back to https://developer.amazon.com/home.html and sign in as needed
* ii. Go to Alexa > Alexa Skills Kit > the Alexa skill you created earlier
* iii. In the Configuration tab:
* iv. Lambda ARN default = enter your <a href="#LambdaARN" target="_self">Lambda ARN</a> noted in the next section.
* v. Authorization URI = https://www.amazon.com/ap/oa
* vi. Access Token URI: https://api.amazon.com/auth/o2/token
* vii. Client ID = your client ID from LWA noted in a previous step
* viii.	Client Secret: your client secret from LWA noted in a previous step
* ix. Client Authentication Scheme: HTTP Basic (Recommended)
* x. Scope: profile (click Add Scope first to add)
* xi. Click Save
* xii. Provide Redirect URL's to LWA:
* xiii. The Configuration page for your Skill lists several Redirect URL's. Open the LWA security profile you created earlier and visit the Web Settings dialog. Provide each of the Redirect URL values from your Skill in the “Allowed Return URLs” field.



After mutual association, the second step would be account linking. On the Alexa console, click on the account linking option, input the authorization URI and the access token URI as shown below.

<div align="center">
  <img src="./images/account_linking.png">
  <center> <b>Figure: Account linking</b> </center>
</div>  

The account linking in this project is using Login with Amazon(LWA), which gives the log in the authorization right to Amazon. If a third-platform is used to communicate with Alexa, the account linking must be set using Aouth2.0. Because this project doesn’t have a device cloud, so here we just use LWA.
Amazon console also supports the skill-testing. Click on the “Test” option; another interface comes out.

<div align="center">
  <img src="./images/skill_test.png">
  <center> <b>Figure: Skill-testing</b> </center>
</div>  

<div align="center">
  <img src="./images/config_the_skill_1.png">
</div>  

<div align="center">
  <img src="./images/config_the_skill_2.png">
</div>  

6.	Go to https://alexa.amazon.com/spa/index.html#skills

<div align="center">
  <img src="./images/skill_configuration_result.png">
</div>  
</br>

7. Test your Alexa Skill
The developer can choose either typing or using JSON formatted directive to test the Alexa skill. It simulates a real echo plus speaker to send the directives to the server where holding the Alexa skill.
For example, if the user type or say “turn on my light”, the Alexa server will send a turn-on directive to the lambda(the skill holder server).

<div align="center">
  <img src="./images/test_turn_on_a_light.png">
  <center> <b>Figure: Turn on a light</b> </center>
</div>  

<div align="center">
  <img src="./images/test_turn_on_jSON_directive.png">
  <center> <b>Figure: TurnOn JSON directive</b> </center>
</div>  

Then the lambda function will handle the directive and give a response.

<div align="center">
  <img src="./images/test_turn_on_jSON_directive_response.png">
  <center> <b>Figure: TurnOn response</b> </center>
</div>  

The keywords “event” and “directive” on the top of the JSON file marks what the message is and where does the message from.

###	Lambda function:
Clone the repo https://github.com/sheldon123z/Alexa-esp32freeRTOS-EFR32BG13-Bluetooth-mesh-project.git
Zip all contents of that directory to python.zip. 

**Create the lambda function:**
1.	Go to https://console.aws.amazon.com/console/home and sign in
2.	Switch the AWS server to “US East (N.Virginia)” on the top right of the page, because some of the features are only supported on this server.
3.	Go to Services > Compute > Lambda
<div align="center">
  <img src="./images/aws_management_console.png">
</div>  

4.	Click on Create Function
<div align="center">
  <img src="./images/create_asw_lambda_function.png">
</div>  
</br>

5.	Click on “Author from scratch”
6.	Configure your Lambda function
* Name = SampleLambdaFunction (or whatever you want)
* Runtime = Python 3.6 (You can choose any language used to write your function, in this demo, we are using Python 3.6)
* Permission = “Create a new role with basic Lambda permissions”
* Click Create Function

<div align="center">
  <img src="./images/configure_lambda_function.png">
</div>  
</br>

7.	Click Triggers -> Add Trigger and select **Alexa Smart Home**
* Application Id = <a href="#YourSkillID" target="_self">skill ID</a> of your test skill that your created above.
* Enable trigger = checked
* Click Submit

<div align="center">
  <img src="./images/lambda_trigger_setting_1.png">
</div>  

<div align="center">
  <img src="./images/lambda_trigger_setting_2.png">
</div>  
</br>
 
8.	Also need to add the “DynamoDB” as another trigger. (no need now if there is no database access)
9.	Click the icon of the lambda function for Configuration

<div align="center">
  <img src="./images/lambda_function_configuration.png">
</div>  
</br>

Regarding to the lambda function, we take the *Alexa Smart Home skill sample* released by Amazon as starting point, you can access it from the repository [skill-sample-python-smarthome-switch](https://github.com/alexa/skill-sample-python-smarthome-switch). And also you can just reuse the lambda function we have implemented for this project, the zip package ***alexa-control-btmesh-lambda.zip*** is included in the sub directory. 

* Runtime = Python 3.6
* Code entry type = Upload a .ZIP file (the Alexa skill package). After uploading the .ZIP file, please make sure that the self_defined_lambda.py in the root folder.
* Click on Upload and find the python.zip you created earlier
* Handler = self_defined_lambda.lambda_handler
* Click Next
* Click Save
* On the top right corner, note the <span id = "LambdaARN"><font color="red">Lambda ARN</font></span>.
* Lambda ARN: Copy the ARN and it will be used in the Alexa Skill section as mentioned above. 

<div align="center">
  <img src="./images/lambda_function_configuration_2.png">
</div>  

10.	Setting Up Your AWS Account and Permissions with IAM
IAM (AWS Identity and Access Management) is a web service that helps you securely control access to AWS resources. You use IAM to control who is authenticated (signed in) and authorized (has permissions) to use resources. Please see the [user guide](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-account-and-permissions.html) for how to set up your account and permission with IAM.
To grant your IAM user account access to AWS IoT and Amazon FreeRTOS, attach the following IAM policies to your IAM user account:
* AmazonFreeRTOSFullAccess
* AWSIoTFullAccess  

Find the IAM services
<div align="center">
  <img src="./images/find_the_iam_services.png">
</div>  


##  Download and set up freeRTOS SDK on ESP32:
### Download the project
Go to GitHub page to download the SDK or clone it by using git clone https://github.com/aws/amazon-freertos.git

### Use CMake to generate project build files and build project
Setup the SDK with correct Toolchains and establish the serial connection to your ESP32 board. The very detailed official instruction can be found HERE [EN](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_espressif.html) | [CN](https://docs.aws.amazon.com/zh_cn/freertos/latest/userguide/getting_started_espressif.html).

#### Install ESP-IDF 
Read the page here [EN](https://docs.espressif.com/projects/esp-idf/en/v3.1.5/get-started-cmake/windows-setup.html) for how to get start with CMake on windows. 
ESP-IDF is the official development framework for the ESP32 chip. Please note that due to some compatibility issue, the version 3.1.5 of ESP-IDF (the version used by Amazon FreeRTOS) does not support the latest version of the ESP32 compiler. You must use the compiler that is compatible with version 3.1.5 of the ESP-IDF. The compatible compiler is provided within the installation package ```esp-idf-tools-setup-1.1.exe```, please get the installation package from the link above.

The installer will automatically install the **ESP32 Xtensa gcc toolchain**, **Ninja** build tool, and a configuration tool called **mconf-idf**. The installer can also download and run installers for CMake and Python 2.7 if these are not already installed on the computer. However, the CMake included in the installer is an elder version, for CMake installing method, please read the section [Install CMake](#install-cmake) below.

<div align="center">
<img src="./images/esp_idf_tool_setup_0.png">
</div>  
</br>

After setup has finished installing ESP-IDF tools, the GUI now should looks like this. 
   
<div align="center">
<img src="./images/esp_idf_tool_setup.png">
</div>  
</br>

Click the **Finish** button, ESP-IDF will set the system environment automatically.
<div align="center">
<img src="./images/esp_idf_tool_set_environment.png">
</div>  
</br>  

By default, the installer updates the Windows Path environment variable so all of these tools can be run from anywhere.
<div align="center">
<img src="./images/installer_update_path_environment.png">
</div>  
</br>  

If you didn't see these path included in the System Environment, you will need to configure the environment where you are using ESP-IDF with the correct paths by yourself.

#### Install CMake

Install the CMake (Cross-Platform Makefile Generator). The CMake build system is required to build the Amazon FreeRTOS demo and test applications for this device. Amazon FreeRTOS supports versions 3.13 and later.

Form you native build system, it can be GNU Make or Ninja. Ninja is recommended by Amazon since it is faster than Make and also provides native support to all desktop operating systems. And we also use the Ninja in this project.

#### Generate build file by CMake
Use the following CMake command to generate the build files, and then use ```ninja``` to build the application.
```cmake -DVENDOR=espressif -DBOARD=esp32_devkitc -DCOMPILER=xtensa-esp32 -GNinja -S . -B your-build-directory```

#### To build the application
Change directories to the build directory ```your-build-directory``` you just specified above, i.e, ```build```.
Invoke Ninja to build the application.
```ninja```

<div align="center">
<img src="./images/ninja_build_project.png">
</div>  
</br>

#### Flash and Run Amazon FreeRTOS
Establish serial connection between your host machine and the ESP32 kit. You must install CP210x USB to UART Bridge VCP drivers. You can download these [drivers](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers) from Silicon Labs.

Use Espressif's IDF utility ( amazon-freertos/vendors/espressif/esp-idf/tools/idf.py ) to flash your board, run the application, and see logs.
To erase the board's flash, go to the <amazon-freertos> directory and use the following command:  
```python ./vendors/espressif/esp-idf/tools/idf.py erase_flash -p COM17 -B build```

To flash the application binary to your board, use the IDF script to flash your board:
```python ./vendors/espressif/esp-idf/tools/idf.py flash -p COM17 -B build```

To monitor:
```python ./vendors/espressif/esp-idf/tools/idf.py monitor -p COM17 -B build```

You can combine these commands. For example, suppose the com port of your ESP32 kit is COM5:
```python ./vendors/espressif/esp-idf/tools/idf.py erase_flash flash monitor -p COM17 -B build```  

### Add the custom source code

Please carefully read the [documentation page](https://docs.aws.amazon.com/freertos/latest/userguide/freertos-getting-started.html) on how to setup the AWS freeRTOS SDK on your computer. The chinese version documentation is available [here]((https://docs.aws.amazon.com/zh_cn/freertos/latest/userguide/freertos-getting-started.html)).  

Open the SDK folder, in the folder …/demos/shadow, replace the C file “aws_iot_demo_shadow.c” with the provided “aws_iot_demo_shadow.c” file. Also, add the file “aws_iot_shadow_blem.h” to the folder.
d.	The WiFi settings can be done in the file …/demos/include/aws_clientcrediential.h
e.	Paste the certificate you acquired when you create the “esp32” thing on AWS IoT console to the file …/demos/includes/aws_clientcrediential_keys.h
f.	Alternatively, you can use the quick setup which instructed on [this page](https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_espressif.html) under Configure the Amazon FreeRTOS Demo Applications section.

###	Build the esp32 program 
Serial connection to your ESP32 board
a.	In the file:
b.	…/vendors/espressif/boards/esp32/aws_demos/config_files/aws_demo_config.h input. #define CONFIG_SHADOW_DEMO_ENABLED
c.	Follow the building instruction on the manual page to build and flash the application.

## Build BG13 embedded provisioner program
a.	Clone the project from GitHub using “git clone https://github.com/sheldon123z/EFR32-embedded-provisioner.git”
b.	Open the project in simplicity studio, build, flash to BG13 board

## Build MG21 Bluetooth mesh light/switch/sensor/sensor monitor program
a.	The mesh light and switch are using the demo applications in the simplicity studio Bluetooth mesh SDK v1.5.0, you can use other Bluetooth mesh light device in this project.
b.	The mesh switch application is also the demo program in simplicity studio, it has three functions: adjusting the lightness of the light, adjusting the temperature, turning on/off the light. A long press will turn on/off the light, a short press will adjust the lightness, and a normal press will change the temperature. Currently, the provisioner only subscribes the on/off function, when the user give it a short press or long press, the provisioner will report the light’s on/off status to IoT console(while the lightness is not 0, the light’s on/off state will be “ON”).
c.	Sensor/monitor program. The sensor/monitor program is only available after the version 1.5.0 of the Bluetooth mesh SDK. Currently, the IoT console doesn’t monitor the state of the sensor. Once the sensor and the monitor are provisioned, they are functioning as local devices. 


8.	Bluetooth mesh network operation guide:
a.	Flash the provisioner program to the BG13 board.
b.	Hold left button and reset button to reset the board
c.	Flash the switch program and light program into board
d.	Open a COM tool and connect it to the BG13 board, when seeing the prompt to provision the switch board and light board, press left button to provision.
e.	After the switch and light board are provisioned, the user can use phone app or the echo plus 2 to control the light.

9.	Application addresses
a.	freeRTOS SDK(including the application file) https://github.com/sheldon123z/amazon-freertos-shadow
b.	embedded provisioner(app.c app.h cJSON.c cJSON.h main.c)
https://github.com/sheldon123z/EFR32-embedded-provisioner.git
c.	Alexa skill (use zip to compress and upload to lambda)
https://github.com/sheldon123z/Alexa-esp32freeRTOS-EFR32BG13-Bluetooth-mesh-project.git
d.	Light and Swtich applications are the demo applications

# Conclusion:
This project uses multiple protocols and Amazon AWS services. It breaks the limitation between the Zigbee embedded smart speaker and Bluetooth mesh embedded smart devices and realizes voice control. Although it is just a brief demonstration, it reveals the potential of multi-platform cooperation and smart automation.
