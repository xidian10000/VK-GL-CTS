/*-------------------------------------------------------------------------
 * OpenGL Conformance Test Suite
 * -----------------------------
 *
 * Copyright (c) 2022-2022 The Khronos Group Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/*!
 * \file  esextcFragmentShadingRateErrors.hpp
 * \brief FragmentShadingRateEXT errors
 */ /*-------------------------------------------------------------------*/

#include "esextcFragmentShadingRateErrors.hpp"
#include "gluContextInfo.hpp"
#include "gluDefs.hpp"
#include "glwEnums.hpp"
#include "glwFunctions.hpp"
#include "tcuTestLog.hpp"
#include <cstddef>

namespace glcts
{

/// Constructor
///
/// @param context     Test context
/// @param name        Test case's name
/// @param description Test case's description
FragmentShadingRateErrors::FragmentShadingRateErrors(Context& context, const ExtParameters& extParams, const char* name,
													 const char* description)
	: TestCaseBase(context, extParams, name, description)
{
}

/// Initialize the test.
void FragmentShadingRateErrors::init(void)
{
	TestCaseBase::init();

	// Skip if required extensions are not supported.
	if (!m_is_fragment_shading_rate_supported)
	{
		throw tcu::NotSupportedError(FRAGMENT_SHADING_RATE_NOT_SUPPORTED, "", __FILE__, __LINE__);
	}
}

/// Deinitializes all GLES objects created for the test.
void FragmentShadingRateErrors::deinit(void)
{
	// Deinitialize base class
	TestCaseBase::deinit();
}

/// Test if the error code returned by glGetError is the same as expected.
///  If the error is different from expected description is logged.
///
/// @param expected_error    GLenum error which is expected
/// @param description       Log message in the case of failure.
///
/// @return true if error is equal to expected, false otherwise.
glw::GLboolean FragmentShadingRateErrors::verifyError(const glw::GLenum expected_error, const char* description) const
{
	/* Retrieve GLES entry points. */
	const glw::Functions& gl = m_context.getRenderContext().getFunctions();

	glw::GLboolean test_passed = true;
	glw::GLenum	   error_code  = gl.getError();

	if (error_code != expected_error)
	{
		test_passed = false;

		m_testCtx.getLog() << tcu::TestLog::Message << description << tcu::TestLog::EndMessage;
	}

	return test_passed;
}

/// Executes the test.
/// Sets the test result to QP_TEST_RESULT_FAIL if the test failed, QP_TEST_RESULT_PASS otherwise.
/// Note the function throws exception should an error occur!
///
///  @return STOP if the test has finished, CONTINUE to indicate iterate should be called once again.
tcu::TestNode::IterateResult FragmentShadingRateErrors::iterate(void)
{
	bool testPassed = true;

	/* Retrieve GLES entry points. */
	const glw::Functions& gl = m_context.getRenderContext().getFunctions();

	// Shading Rate                   Size
	// ----------------------------   -----
	// SHADING_RATE_1X1_PIXELS_EXT    1x1
	// SHADING_RATE_1X2_PIXELS_EXT    1x2
	// SHADING_RATE_1X4_PIXELS_EXT    1x4
	// SHADING_RATE_2X1_PIXELS_EXT    2x1
	// SHADING_RATE_2X2_PIXELS_EXT    2x2
	// SHADING_RATE_2X4_PIXELS_EXT    2x4
	// SHADING_RATE_4X1_PIXELS_EXT    4x1
	// SHADING_RATE_4X2_PIXELS_EXT    4x2
	// SHADING_RATE_4X4_PIXELS_EXT    4x4
	// INVALID_ENUM is generated by ShadingRateEXT if <rate> is not a valid shading rate from table
	gl.shadingRateEXT(GL_SAMPLE_SHADING);
	testPassed = testPassed && verifyError(GL_INVALID_ENUM, "glShadingRateEXT <rate> is not valid");

	if (m_is_fragment_shading_rate_attachment_supported)
	{
		// void FramebufferShadingRateEXT(enum target, enum attachment, uint texture, GLint baseLayer, GLsizei numLayers, GLsizei texelWidth, GLsizei texelHeight);
		constexpr deUint32 kBaseLayer	  = 0;
		constexpr deUint32 kNumLayer	  = 1;
		constexpr deUint32 kTextureWidth  = 256;
		constexpr deUint32 kTextureHeight = 256;
		constexpr deUint32 kTexelWidth	  = 16;
		constexpr deUint32 kTexelHeight	  = 16;

		glw::GLuint fbo_id;
		/* Generate framebuffer objects */
		gl.genFramebuffers(1, &fbo_id);
		GLU_EXPECT_NO_ERROR(gl.getError(), "Error setting up framebuffer objects");

		gl.bindFramebuffer(GL_FRAMEBUFFER, fbo_id);
		GLU_EXPECT_NO_ERROR(gl.getError(), "Error binding frame buffer object!");

		glw::GLuint to_id;
		glw::GLuint mutable_to_id;
		/* Generate texture objects */
		gl.genTextures(1, &to_id);
		GLU_EXPECT_NO_ERROR(gl.getError(), "Error generating texture objects");

		gl.genTextures(1, &mutable_to_id);
		GLU_EXPECT_NO_ERROR(gl.getError(), "Error generating texture objects");

		gl.bindTexture(GL_TEXTURE_2D, to_id);
		GLU_EXPECT_NO_ERROR(gl.getError(), "Error binding texture object!");
		gl.texStorage2D(GL_TEXTURE_2D, 1, GL_R8UI, kTextureWidth, kTextureHeight);
		GLU_EXPECT_NO_ERROR(gl.getError(), "Error allocating texture object!");

		gl.bindTexture(GL_TEXTURE_2D, mutable_to_id);
		GLU_EXPECT_NO_ERROR(gl.getError(), "Error binding texture object!");
		gl.texImage2D(GL_TEXTURE_2D, 0, GL_R8, kTextureWidth, kTextureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
		GLU_EXPECT_NO_ERROR(gl.getError(), "Error allocating texture object!");

		glw::GLint minTexelWidth		= 0;
		glw::GLint maxTexelWidth		= 0;
		glw::GLint minTexelHeight		= 0;
		glw::GLint maxTexelHeight		= 0;
		glw::GLint maxAttachAspectRatio = 0;
		glw::GLint maxAttachLayerCount	= 0;
		gl.getIntegerv(GL_MIN_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_WIDTH_EXT, &minTexelWidth);
		GLU_EXPECT_NO_ERROR(gl.getError(),
							"Error getIntegerv GL_MIN_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_WIDTH_EXT!");
		gl.getIntegerv(GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_WIDTH_EXT, &maxTexelWidth);
		GLU_EXPECT_NO_ERROR(gl.getError(),
							"Error getIntegerv GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_WIDTH_EXT!");
		gl.getIntegerv(GL_MIN_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_HEIGHT_EXT, &minTexelHeight);
		GLU_EXPECT_NO_ERROR(gl.getError(),
							"Error getIntegerv GL_MIN_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_HEIGHT_EXT!");
		gl.getIntegerv(GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_HEIGHT_EXT, &maxTexelHeight);
		GLU_EXPECT_NO_ERROR(gl.getError(),
							"Error getIntegerv GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_HEIGHT_EXT!");
		gl.getIntegerv(GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_ASPECT_RATIO_EXT, &maxAttachAspectRatio);
		GLU_EXPECT_NO_ERROR(gl.getError(),
							"Error getIntegerv GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_ASPECT_RATIO_EXT!");
		gl.getIntegerv(GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_LAYERS_EXT, &maxAttachLayerCount);
		GLU_EXPECT_NO_ERROR(gl.getError(), "Error getIntegerv GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_LAYERS_EXT!");

		// An INVALID_ENUM error is generated if <target> is not DRAW_FRAMEBUFFER, READ_FRAMEBUFFER, or FRAMEBUFFER.
		gl.framebufferShadingRateEXT(GL_RENDERBUFFER, GL_SHADING_RATE_ATTACHMENT_EXT, to_id, kBaseLayer, kNumLayer,
									 kTexelWidth, kTexelHeight);
		testPassed = testPassed && verifyError(GL_INVALID_ENUM, "framebufferShadingRateEXT <target> is not valid");

		// An INVALID_ENUM error is generated if <attachment> is not SHADING_RATE_ATTACHMENT_EXT.
		gl.framebufferShadingRateEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, to_id, kBaseLayer, kNumLayer, kTexelWidth,
									 kTexelHeight);
		testPassed = testPassed && verifyError(GL_INVALID_ENUM, "framebufferShadingRateEXT <attachment> is not valid");

		// An INVALID_VALUE error is generated if <texture> is not zero and is not the name of an immutable texture object.
		gl.framebufferShadingRateEXT(GL_FRAMEBUFFER, GL_SHADING_RATE_ATTACHMENT_EXT, mutable_to_id, kBaseLayer,
									 kNumLayer, kTexelWidth, kTexelHeight);
		testPassed = testPassed && verifyError(GL_INVALID_VALUE, "framebufferShadingRateEXT <texture> is not valid");

		// An INVALID_VALUE error is generated if <baseLayer> is greater than or equal to the value of MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_LAYERS_EXT.
		gl.framebufferShadingRateEXT(GL_FRAMEBUFFER, GL_SHADING_RATE_ATTACHMENT_EXT, to_id,
									 GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_LAYERS_EXT, kNumLayer, kTexelWidth,
									 kTexelHeight);
		testPassed = testPassed && verifyError(GL_INVALID_VALUE, "framebufferShadingRateEXT <baseLayer> is not valid");

		// An INVALID_VALUE error is generated if <numLayers> is greater than the value of MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_LAYERS_EXT.
		gl.framebufferShadingRateEXT(GL_FRAMEBUFFER, GL_SHADING_RATE_ATTACHMENT_EXT, to_id, 0,
									 GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_LAYERS_EXT + 1, kTexelWidth, kTexelHeight);
		testPassed = testPassed && verifyError(GL_INVALID_VALUE, "framebufferShadingRateEXT <numLayers> is not valid");

		// An INVALID_VALUE error is generated if <texelWidth> / <texelHeight> is larger than MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_ASPECT_RATIO_EXT.
		gl.framebufferShadingRateEXT(GL_FRAMEBUFFER, GL_SHADING_RATE_ATTACHMENT_EXT, to_id, 0,
									 GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_LAYERS_EXT + 1, minTexelWidth,
									 (minTexelWidth * GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_ASPECT_RATIO_EXT)
										 << 1);
		testPassed = testPassed &&
					 verifyError(GL_INVALID_VALUE, "framebufferShadingRateEXT <texelWidth, texelHeight> is not valid");

		// An INVALID_VALUE error is generated if <texelHeight> / <texelWidth> is larger than MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_ASPECT_RATIO_EXT.
		gl.framebufferShadingRateEXT(
			GL_FRAMEBUFFER, GL_SHADING_RATE_ATTACHMENT_EXT, to_id, 0,
			GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_LAYERS_EXT + 1,
			(minTexelHeight * GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_ASPECT_RATIO_EXT) << 1, minTexelHeight);
		testPassed = testPassed &&
					 verifyError(GL_INVALID_VALUE, "framebufferShadingRateEXT <texelWidth, texelHeight> is not valid");

		gl.deleteFramebuffers(1, &fbo_id);
		gl.deleteFramebuffers(1, &to_id);
		gl.deleteFramebuffers(1, &mutable_to_id);
	}

	// void ShadingRateCombinerOpsEXT(enum combinerOp0, enum combinerOp1)
	//
	// An INVALID_ENUM error is generated if <combinerOp0> is not
	// An INVALID_ENUM error is generated if <combinerOp1> is not
	// FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_EXT,
	// FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_EXT,
	// FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_EXT,
	// FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_EXT, or
	// FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_EXT
	gl.shadingRateCombinerOpsEXT(GL_SHADING_RATE_EXT, GL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_EXT);
	testPassed = testPassed && verifyError(GL_INVALID_ENUM, "shadingRateCombinerOpsEXT <combinerOp0> is not valid");
	gl.shadingRateCombinerOpsEXT(GL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_EXT,
								 GL_MIN_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_WIDTH_EXT);
	testPassed = testPassed && verifyError(GL_INVALID_ENUM, "shadingRateCombinerOpsEXT <combinerOp1> is not valid");

	glw::GLboolean supportNonTrivialCombiner = false;
	gl.getBooleanv(GL_FRAGMENT_SHADING_RATE_NON_TRIVIAL_COMBINERS_SUPPORTED_EXT, &supportNonTrivialCombiner);
	GLU_EXPECT_NO_ERROR(gl.getError(), "Error getBooleanv non trivial combiner");

	// An INVALID_OPERATION error is generated if the value of FRAGMENT_SHADING_RATE_NON_TRIVIAL_COMBINERS_SUPPORTED_EXT
	// is FALSE and <combinerOp0> is not
	// FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_EXT or FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_EXT
	// is FALSE and <combinerOp1> is not
	// FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_EXT or FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_EXT
	if (!supportNonTrivialCombiner)
	{
		gl.shadingRateCombinerOpsEXT(GL_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_EXT,
									 GL_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_EXT);
		testPassed = testPassed && verifyError(GL_INVALID_OPERATION, "<combinerOp0> combiner is non trivial combiner");

		gl.shadingRateCombinerOpsEXT(GL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_EXT,
									 GL_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_EXT);
		testPassed = testPassed && verifyError(GL_INVALID_OPERATION, "<combinerOp1> combiner is non trivial combiner");
	}
	// [[If GL_EXT_fragment_shading_rate_primitive is not supported]]
	// An INVALID_OPERATION error is generated if <combinerOp0> is not
	// FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_EXT
	if (!m_is_fragment_shading_rate_primitive_supported)
	{
		gl.shadingRateCombinerOpsEXT(GL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_EXT,
									 GL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_EXT);
		testPassed = testPassed && verifyError(GL_INVALID_ENUM, "shadingRateCombinerOpsEXT <combinerOp0> is not valid");
	}

	// [[If GL_EXT_fragment_shading_rate_attachment is not supported]]
	// An INVALID_OPERATION error is generated if <combinerOp1> is not
	// FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_EXT
	if (!m_is_fragment_shading_rate_attachment_supported)
	{
		gl.shadingRateCombinerOpsEXT(GL_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_EXT,
									 GL_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_EXT);
		testPassed = testPassed && verifyError(GL_INVALID_ENUM, "shadingRateCombinerOpsEXT <combinerOp0> is not valid");
	}

	/* All done */
	if (testPassed)
	{
		m_testCtx.setTestResult(QP_TEST_RESULT_PASS, "Pass");
	}
	else
	{
		m_testCtx.setTestResult(QP_TEST_RESULT_FAIL, "Fail");
	}

	return STOP;
}

} // namespace glcts