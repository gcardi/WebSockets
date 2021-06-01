object frmMain: TfrmMain
  Left = 0
  Top = 0
  Caption = 'frmMain'
  ClientHeight = 381
  ClientWidth = 635
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    635
    381)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 509
    Top = 277
    Width = 81
    Height = 13
    Alignment = taRightJustify
    Anchors = [akLeft, akRight]
    Caption = 'M.F. Chunk Size:'
  end
  object Label2: TLabel
    Left = 509
    Top = 250
    Width = 62
    Height = 13
    Alignment = taRightJustify
    Anchors = [akLeft, akRight]
    Caption = 'M.F. Length:'
  end
  object Label3: TLabel
    Left = 25
    Top = 27
    Width = 49
    Height = 13
    Alignment = taRightJustify
    Anchors = [akLeft, akRight]
    Caption = 'Base URL:'
  end
  object edtURL: TEdit
    Left = 80
    Top = 24
    Width = 417
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
    Text = 'http://localhost:8080/websocket'
  end
  object Button1: TButton
    Left = 512
    Top = 22
    Width = 115
    Height = 25
    Action = actWebSocketsSendSFText
    Anchors = [akTop, akRight]
    TabOrder = 1
  end
  object PageControl1: TPageControl
    Left = 8
    Top = 64
    Width = 489
    Height = 292
    ActivePage = tbshtLog
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 2
    object tbshtLog: TTabSheet
      Caption = 'Log'
      inline frmeReLog1: TfrmeReLog
        Left = 0
        Top = 0
        Width = 481
        Height = 264
        Align = alClient
        TabOrder = 0
        ExplicitWidth = 481
        ExplicitHeight = 264
        inherited RichEdit1: TRichEdit
          Width = 481
          Height = 264
          ExplicitWidth = 481
          ExplicitHeight = 264
        end
      end
    end
    object tbshtRequest: TTabSheet
      Caption = 'Request'
      ImageIndex = 3
      object reRequest: TRichEdit
        Left = 0
        Top = 0
        Width = 481
        Height = 264
        Align = alClient
        BevelOuter = bvNone
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Consolas'
        Font.Style = []
        ParentFont = False
        PlainText = True
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
        Zoom = 100
      end
    end
    object tbshtResponse: TTabSheet
      Caption = 'Response'
      ImageIndex = 1
      object reResponse: TRichEdit
        Left = 0
        Top = 0
        Width = 481
        Height = 264
        Align = alClient
        BevelOuter = bvNone
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Consolas'
        Font.Style = []
        ParentFont = False
        PlainText = True
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
        Zoom = 100
      end
    end
    object tbshtBody: TTabSheet
      Caption = 'Body'
      ImageIndex = 2
      object reBody: TRichEdit
        Left = 0
        Top = 0
        Width = 481
        Height = 264
        Align = alClient
        BevelOuter = bvNone
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Consolas'
        Font.Style = []
        ParentFont = False
        PlainText = True
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
        Zoom = 100
      end
    end
  end
  object edtSendText: TEdit
    Left = 512
    Top = 84
    Width = 115
    Height = 21
    Anchors = [akTop, akRight]
    TabOrder = 3
    Text = 'ABCD'
  end
  object Button2: TButton
    Left = 512
    Top = 134
    Width = 115
    Height = 25
    Action = actWebSocketsSendSFBinary
    Anchors = [akTop, akRight]
    TabOrder = 4
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 362
    Width = 635
    Height = 19
    AutoHint = True
    Panels = <>
  end
  object edtSendBinary: TEdit
    Left = 512
    Top = 197
    Width = 115
    Height = 21
    Anchors = [akTop, akRight]
    TabOrder = 6
    Text = 'ABCD'
  end
  object Button3: TButton
    Left = 512
    Top = 53
    Width = 115
    Height = 25
    Action = actWebSocketsSendMFText
    Anchors = [akTop, akRight]
    TabOrder = 7
  end
  object edtMFLength: TEdit
    Left = 573
    Top = 247
    Width = 54
    Height = 21
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    NumbersOnly = True
    TabOrder = 8
    Text = '131072'
  end
  object edtMFChunkSize: TEdit
    Left = 592
    Top = 274
    Width = 35
    Height = 21
    Alignment = taRightJustify
    Anchors = [akTop, akRight]
    NumbersOnly = True
    TabOrder = 9
    Text = '512'
  end
  object Button4: TButton
    Left = 512
    Top = 165
    Width = 115
    Height = 25
    Action = actWebSocketsSendSFBinary
    Anchors = [akTop, akRight]
    TabOrder = 10
  end
  object ActionManager1: TActionManager
    Left = 176
    Top = 120
    StyleName = 'Platform Default'
    object actWebSocketsSendSFText: TAction
      Category = 'WebSockets'
      Caption = 'Send S.F. Text'
      Hint = 'Send single frame text message'
      OnExecute = actWebSocketsSendSFTextExecute
      OnUpdate = actWebSocketsSendSFTextUpdate
    end
    object actWebSocketsSendSFBinary: TAction
      Category = 'WebSockets'
      Caption = 'Send S.F. Binary'
      Hint = 'Send single frame binary message'
      OnExecute = actWebSocketsSendSFBinaryExecute
      OnUpdate = actWebSocketsSendSFBinaryUpdate
    end
    object actWebSocketsSendMFText: TAction
      Category = 'WebSockets'
      Caption = 'Send M.F. Text'
      Hint = 'Send multiple frames text message'
      OnExecute = actWebSocketsSendMFTextExecute
      OnUpdate = actWebSocketsSendMFTextUpdate
    end
  end
end
