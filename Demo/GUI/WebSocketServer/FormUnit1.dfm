object Form1: TForm1
  Left = 271
  Top = 114
  Caption = 'Form1'
  ClientHeight = 466
  ClientWidth = 578
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  TextHeight = 13
  object RichEdit1: TRichEdit
    Left = 0
    Top = 153
    Width = 578
    Height = 313
    Align = alClient
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Consolas'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitLeft = 3
    ExplicitTop = 160
    ExplicitWidth = 588
    ExplicitHeight = 350
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 578
    Height = 153
    Align = alTop
    BevelOuter = bvNone
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 1
    ExplicitWidth = 594
    object Label1: TLabel
      Left = 24
      Top = 48
      Width = 20
      Height = 13
      Caption = 'Port'
    end
    object ButtonOpenBrowser: TButton
      Left = 24
      Top = 112
      Width = 107
      Height = 25
      Caption = 'Open Browser'
      TabOrder = 0
      OnClick = ButtonOpenBrowserClick
    end
    object ButtonStart: TButton
      Left = 24
      Top = 8
      Width = 75
      Height = 25
      Caption = 'Start'
      TabOrder = 1
      OnClick = ButtonStartClick
    end
    object ButtonStop: TButton
      Left = 105
      Top = 8
      Width = 75
      Height = 25
      Caption = 'Stop'
      TabOrder = 2
      OnClick = ButtonStopClick
    end
    object cboxLogFlow: TCheckBox
      Left = 152
      Top = 116
      Width = 97
      Height = 17
      Caption = 'Log Flow'
      Checked = True
      State = cbChecked
      TabOrder = 3
      OnClick = cboxLogFlowClick
    end
    object EditPort: TEdit
      Left = 24
      Top = 67
      Width = 121
      Height = 21
      TabOrder = 4
      Text = '8080'
    end
  end
  object ApplicationEvents1: TApplicationEvents
    OnIdle = ApplicationEvents1Idle
    Left = 288
    Top = 24
  end
end
