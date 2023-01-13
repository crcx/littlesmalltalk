"
	this is useful only if the plot(3) routines work
"
Class Main
| bic show |
[
	init
		bic <- Pen new.
		show <- PenShow new.
		show withPen: bic.
		bic extent: 0 @ 0 to: 500 @ 500.
|
	main
		self init.
		self polyShow.
		self spiralShow.
		self formShow.
|
	polyShow
		bic erase.
		bic up.
		bic goTo: 50 @ 50.
		bic down.
		(3 to: 8) do: [:i |
			show poly: i length: 10 ].
|
	spiralShow
		bic erase.
		bic up.
		bic goTo: 250 @ 250.
		bic down.
		show spiral: 150 angle: 89
|
	formShow	| newForm saveBic |
		newForm <- Form new.
		saveBic <- bic.
		bic <- PenSave new.
		bic setForm: newForm.
		bic direction: 0.0.
		bic down.
		show withPen: bic.
		self polyShow.
		bic <- saveBic.
		bic down.
		newForm with: bic displayAt: -15 @ ( -15 ).
		newForm with: bic displayAt: 0 @ 0.
		newForm with: bic displayAt: 20 @ ( -20 ).
		^ newForm
]
