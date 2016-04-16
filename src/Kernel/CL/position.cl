
kernel void positionKernel(write_only image2d_t output){
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	int w = get_image_width(output);
	int h = get_image_height(output);
	write_imagef(
		output,globalID,
		(float4)(
			3 * ((float)(globalID.x) - w/2) / w,
			3 * ((float)(globalID.y) - h/2) / h,
			0,
			0
		)
	);
}