





















































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































"""0
#	def show_output_info(self):
#		QMessageBox.information(self, "sx* output","outdir is the name of the output folder specified by the user. If it does not exist, the directory will be created. If it does exist, the program will crash and an error message will come up. Please change the name of directory and restart the program.")
	"""






































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































"""1
# ========================================================================================
class SXDialogCalculator(QDialog):
	def __init__(self, parent = None):
		super(QDialog, self).__init__(parent)
		
		self.setWindowModality(Qt.ApplicationModal)
		
		# self.setWindowTitle()
		self.setWindowTitle("Absolute Frequency Calculator")

		temp_label = QLabel("Calculate absolute frequency [1/Pixel] from resolution [A]", self)
		temp_label.move(50,50)
		
		# Create label widget
		temp_label = QLabel("Resolution [A]", self)
		# temp_label.setMinimumWidth(token_label_min_width)
		# grid_layout.addWidget(temp_label, grid_row, grid_col_origin, token_label_row_span, token_label_col_span)
		temp_label.move(50,100)
		self.edit_res = QLineEdit(self)
		self.edit_res.setText('Enter Resolution Here')
		self.edit_res.move(200,100)

		temp_label = QLabel("Pixel Size [A/Pixel]", self)
		# temp_label.setMinimumWidth(token_label_min_width)
		# grid_layout.addWidget(temp_label, grid_row, grid_col_origin, token_label_row_span, token_label_col_span)
		temp_label.move(50,200)
		self.edit_apix = QLineEdit(self)
		self.edit_apix.setText('Enter Pixel Size Here')
		self.edit_apix.move(200,200)
		
		self.btn_apply = QPushButton("Apply", self)
		self.btn_apply.move(50,300)
		self.btn_cancel = QPushButton("Cancel", self)
		self.btn_cancel.move(200,300)
		# self.connect(cmd_token_restore_widget[widget_index], SIGNAL("clicked()"), partial(self.handle_restore_widget_event, cmd_token, widget_index))
		
		### self.show()
"""


































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































