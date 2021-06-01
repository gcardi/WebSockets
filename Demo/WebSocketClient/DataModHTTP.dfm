object dmHTTP: TdmHTTP
  OldCreateOrder = False
  Height = 150
  Width = 286
  object IdHTTP1: TIdHTTP
    Intercept = IdLogEvent1
    IOHandler = IdSSLIOHandlerSocketOpenSSL1
    OnDisconnected = IdHTTP1Disconnected
    OnConnected = IdHTTP1Connected
    AllowCookies = False
    ProxyParams.BasicAuthentication = False
    ProxyParams.ProxyPort = 0
    Request.ContentLength = -1
    Request.ContentRangeEnd = -1
    Request.ContentRangeStart = -1
    Request.ContentRangeInstanceLength = -1
    Request.Accept = 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8'
    Request.BasicAuthentication = False
    Request.UserAgent = 'Mozilla/3.0 (compatible; Indy Library)'
    Request.Ranges.Units = 'bytes'
    Request.Ranges = <
      item
        StartPos = -1
        EndPos = -1
        SuffixLength = -1
      end>
    HTTPOptions = [hoForceEncodeParams, hoNoProtocolErrorException, hoWantProtocolErrorContent]
    Left = 32
    Top = 24
  end
  object IdSSLIOHandlerSocketOpenSSL1: TIdSSLIOHandlerSocketOpenSSL
    Intercept = IdLogEvent1
    MaxLineAction = maException
    Port = 0
    DefaultPort = 0
    SSLOptions.Method = sslvTLSv1_2
    SSLOptions.SSLVersions = [sslvTLSv1_2]
    SSLOptions.Mode = sslmUnassigned
    SSLOptions.VerifyMode = []
    SSLOptions.VerifyDepth = 0
    Left = 159
    Top = 24
  end
  object IdLogEvent1: TIdLogEvent
    ReplaceCRLF = False
    OnReceived = IdLogEvent1Received
    OnSent = IdLogEvent1Sent
    Left = 71
    Top = 78
  end
end
