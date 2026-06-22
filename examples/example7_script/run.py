from pathlib import Path
import subprocess
#import os
#print("cwd =", os.getcwd())

exe = "../../build/mf_meshconverter.out"

input_mesh = Path("st_base.vtk")
st_cmd_json1 = Path("meshmodi_st1.json")
st_cmd_json2 = Path("meshmodi_st2.json")
st_cmd_json3 = Path("meshmodi_st3.json")
out_tmp1 = Path("tmp1.vtk")
output_mesh = Path("st3D.vtk")

print(input_mesh)

print("convert st1")
subprocess.run([exe, input_mesh, st_cmd_json1, out_tmp1], check=True)
print("convert st2")
subprocess.run([exe, out_tmp1, st_cmd_json2, output_mesh], check=True)




input_mesh = Path("rt_base.vtk")
rt_cmd_json1 = Path("meshmodi_rt1.json")
rt_cmd_json2 = Path("meshmodi_rt2.json")
out_tmp1 = Path("tmp1r.vtk")
output_mesh = Path("rt3D.vtk")

print(input_mesh)

print("convert rt1")
subprocess.run([exe, input_mesh, rt_cmd_json1, out_tmp1], check=True)
print("convert rt2")
subprocess.run([exe, out_tmp1, rt_cmd_json2, output_mesh], check=True)

