SET obj = CreateObject("MemoryNameSrv.NameServer")

MsgBox "An MemoryNameSrv.NameServer object is created"

' call the HelloWorld method that returns a string
MsgBox "The GetSharedMemoryName method returns: " & obj.GetSharedMemoryName
