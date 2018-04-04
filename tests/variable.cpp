// This file is part of asteria.
// Copyleft 2018, LH_Mouse. All wrongs reserved.

#include "test_init.hpp"
#include "../src/variable.hpp"
#include "../src/reference.hpp"
#include "../src/stored_value.hpp"
#include "../src/recycler.hpp"

using namespace Asteria;

int main(){
	auto var = Xptr<Variable>(std::make_shared<Variable>(true));
	ASTERIA_TEST_CHECK(var->get_type() == Variable::type_boolean);
	ASTERIA_TEST_CHECK(var->get<D_boolean>() == true);
	ASTERIA_TEST_CHECK_CATCH(var->get<D_string>());
	ASTERIA_TEST_CHECK(var->get_opt<D_double>() == nullptr);

	var->set(D_integer(42));
	ASTERIA_TEST_CHECK(var->get_type() == Variable::type_integer);
	ASTERIA_TEST_CHECK(var->get<D_integer>() == 42);

	var->set(1.5);
	ASTERIA_TEST_CHECK(var->get_type() == Variable::type_double);
	ASTERIA_TEST_CHECK(var->get<D_double>() == 1.5);

	var->set(std::string("hello"));
	ASTERIA_TEST_CHECK(var->get_type() == Variable::type_string);
	ASTERIA_TEST_CHECK(var->get<D_string>() == "hello");

	std::array<unsigned char, 16> uuid = { 1,2,3,4,5,6,7,8,2,2,3,4,5,6,7,8 };
	Opaque_struct opaque = { uuid, 12345, std::make_shared<char>() };
	var->set(opaque);
	ASTERIA_TEST_CHECK(var->get_type() == Variable::type_opaque);
	ASTERIA_TEST_CHECK(var->get<D_opaque>().uuid == opaque.uuid);
	ASTERIA_TEST_CHECK(var->get<D_opaque>().context == opaque.context);
	ASTERIA_TEST_CHECK(var->get<D_opaque>().handle == opaque.handle);

	D_function function = {
		{ },
		[](Spref<Recycler> recycler, boost::container::vector<Reference> &&params) -> Reference {
			auto param_one = read_reference_opt(params.at(0));
			ASTERIA_TEST_CHECK(param_one);
			auto param_two = read_reference_opt(params.at(1));
			ASTERIA_TEST_CHECK(param_two);
			Xptr<Variable> xptr;
			recycler->set_variable(xptr, param_one->get<D_integer>() * param_two->get<D_integer>());
			Reference::S_rvalue_generic ref = { xptr.release() };
			return std::move(ref);
		}
	};
	var->set(std::move(function));
	ASTERIA_TEST_CHECK(var->get_type() == Variable::type_function);
	boost::container::vector<Reference> params;
	Reference::S_rvalue_generic ref = { std::make_shared<Variable>(D_integer(12)) };
	params.emplace_back(std::move(ref));
	ref = { std::make_shared<Variable>(D_integer(15)) };
	params.emplace_back(std::move(ref));
	auto recycler = std::make_shared<Recycler>();
	auto result = var->get<D_function>().payload(recycler, std::move(params));
	auto rptr = read_reference_opt(result);
	ASTERIA_TEST_CHECK(rptr);
	ASTERIA_TEST_CHECK(rptr->get<D_integer>() == 180);

	D_array array;
	array.emplace_back(Xptr<Variable>(std::make_shared<Variable>(true)));
	array.emplace_back(Xptr<Variable>(std::make_shared<Variable>(std::string("world"))));
	var->set(std::move(array));
	ASTERIA_TEST_CHECK(var->get_type() == Variable::type_array);
	ASTERIA_TEST_CHECK(var->get<D_array>().at(0)->get<D_boolean>() == true);
	ASTERIA_TEST_CHECK(var->get<D_array>().at(1)->get<D_string>() == "world");

	D_object object;
	object.emplace("one", Xptr<Variable>(std::make_shared<Variable>(true)));
	object.emplace("two", Xptr<Variable>(std::make_shared<Variable>(std::string("world"))));
	var->set(std::move(object));
	ASTERIA_TEST_CHECK(var->get_type() == Variable::type_object);
	ASTERIA_TEST_CHECK(var->get<D_object>().at("one")->get<D_boolean>() == true);
	ASTERIA_TEST_CHECK(var->get<D_object>().at("two")->get<D_string>() == "world");
}
