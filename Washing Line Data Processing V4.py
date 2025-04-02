import gspread
import numpy as np
from oauth2client.service_account import ServiceAccountCredentials
import matplotlib.pyplot as plt
from scipy.signal import find_peaks

# Set up credentials
scope = ['https://spreadsheets.google.com/feeds', 'https://www.googleapis.com/auth/drive']
credentials = ServiceAccountCredentials.from_json_keyfile_name('Z:\\Team (Personal Folders)\\Nick Charlton\\PythonProjects\\washing-line-96317414eb79.json', scope)
gc = gspread.authorize(credentials)

# Open the Google Spreadsheet using its title
spreadsheet = gc.open('WashingLineData')  # Replace with your workbook name

# Function to get data from a sheet and convert it to a numpy array
def sheet_to_numpy(sheet_name):
    sheet = spreadsheet.worksheet(sheet_name)
    values = sheet.get_all_values()
    data_array = np.array(values)
    
    # Convert data to float with error handling
    def convert_to_float(value):
        try:
            return float(value)
        except ValueError:
            return np.nan  # Convert non-convertible values to NaN (Not a Number)
    
    return np.vectorize(convert_to_float)(data_array)

# Get data from the three sheets
array1 = sheet_to_numpy('WindSpeed')
array2 = sheet_to_numpy('X Force (Strain1)')
array3 = sheet_to_numpy('Y Force (Strain2)')

# Delete columns from the arrays
array1 = np.delete(array1, [0, 1], axis=1)  # Delete first two columns
array2 = np.delete(array2, 0, axis=1)        # Delete first column
array3 = np.delete(array3, 0, axis=1)        # Delete first column

# Convert the arrays to lists
windSpeed = array1.flatten().tolist()
xAxisForce = array2.flatten().tolist()
yAxisForce = array3.flatten().tolist()

# Function to plot data against time
def plot_data(ax, data, title, ylabel):
    time_axis = range(len(data))  # Time axis with 1-second intervals
    
    # Find peaks in the data
    peaks, _ = find_peaks(data)
    peak_times = [time_axis[i] for i in peaks]
    peak_values = [data[i] for i in peaks]
    
    # Plot a line connecting the peaks with thicker line and dashed style
    ax.plot(peak_times, peak_values, linestyle='--', linewidth=2, label='Peaks Line')  
    
    ax.set_title(title)
    ax.set_xlabel('Time (seconds)')
    ax.set_ylabel(f'{ylabel}')
    ax.legend()
    ax.grid(True)

# Create subplots
fig, axs = plt.subplots(3, 1, figsize=(8, 10))

# Plot Wind Speed
plot_data(axs[0], windSpeed, 'Wind Speed', 'm/s')

# Plot X Axis Force
plot_data(axs[1], xAxisForce, 'X Axis Force', 'kg')

# Plot Y Axis Force
plot_data(axs[2], yAxisForce, 'Y Axis Force', 'kg')

# Adjust layout to prevent overlap
plt.tight_layout()

# Show all plots
plt.show()
