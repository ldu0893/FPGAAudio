# FPGAAudio

Instructions to run:

Clone or download code from github.

Open the project in Quartus II Prime 18.1.

Importantly, open platform designer, locate and open the qsys file in the HDL folder, and generate HDL into the HDL folder.

If necessary, import files, pin assignments, and any other required files and recompile in Quartus.

Program your 10M50DAF484C7G.

Open Nios II Software Build Tools for Eclipse, and create three new Nios II Application and BSP from Template projects.

For each project, use the SOPC information file generated in the HDL folder, and name the projects based on the project folder names in the software folder. They must be exact.

Uncheck "Use default location" and set the project location to the corresponding folder in the software folder.

Use the Blank Project Template.

Once the projects have been generated, make sure to include/import the downloaded c and h files. There are several ways to do this and it will depend on your Eclipse version and other factors.

Build all projects.

Open run configurations, and create three new Nios II Hardware configurations, each with the corresponding project .elf file.

When running, always run sgtl5000_config before any other project.

To play notes using a keyboard, run usb_kb last. To play preprogrammed songs, run FPGAAudio_app last.

# Common issues

If you have mismatched system ID or system timestamp errors when trying to run, go through the whole process once more: Generate HDL -> Compile -> Program -> Generate BSP (x3) -> Build All -> Run

If you have build errors or elf file errors, make sure the pin assignments are correct and that there are no syntax errors in the c code. If you are sure everything is correct, consider restarting Quartus and Eclipse. It has been known to resolve issues. If that doesn't work try recreating the Eclipse projects.

## Note

This was developed as the final project of the UIUC ECE 385 course for Fall 2022. Some material, including SGTL5000 code, USB driver configuration code, and select SV lines were provided and then consequently built upon.

We are not responsible for any cheating that may occur using this respository. ECE 385 is a very fun class, so just don't cheat.