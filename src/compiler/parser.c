/**
 * @file parser.c
 * @brief Purpose: implementation of parser.h 
    which generate a parsing tree using token list and symbol table 
 * @version 1.0
 * @date 04.18.2017
 * @author Ximing
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "utils/parsing_tree.h"
#include "utils/symbol_table.h"
#include "utils/link_list.h"

static void raise_syntax_error(int line_no, char *msg) {
    fprintf(stderr, "Syntax error: in parser line: %d\n %s\n", line_no, msg);
    exit(EINVAL);
}

static parsing_tree_st *generate_decl_stmt(link_list_st *, symbol_table_st *);

static parsing_tree_st *generate_print_stmt(link_list_st *, symbol_table_st *);

static parsing_tree_st *generate_assign_stmt(link_list_st *, symbol_table_st *);

static parsing_tree_st *generate_if_stmt(link_list_st *, symbol_table_st *);

static parsing_tree_st *generate_for_stmt(link_list_st *, symbol_table_st *);

static parsing_tree_st *generate_expre(link_list_st *, symbol_table_st *);

static parsing_tree_st *generate_term(link_list_st *, symbol_table_st *);

static parsing_tree_st *generate_factor(link_list_st *, symbol_table_st *);

static parsing_tree_st *generate_res1(link_list_st *, symbol_table_st *);

static parsing_tree_st *generate_res2(link_list_st *, symbol_table_st *);

static parsing_tree_st *generate_stmt(link_list_st *, symbol_table_st *);

static parsing_tree_st *generate_boolean_expre(link_list_st *, symbol_table_st *);

static parsing_tree_st *generate_boolean_value(link_list_st *, symbol_table_st *);

static parsing_tree_st *generate_stmt_list(link_list_st *, symbol_table_st *);

/**
 * @brief: generate the if statement accordig to the grammar rule
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated if statement
 */
static parsing_tree_st *generate_if_stmt(link_list_st *token_list, symbol_table_st *symbol_table) {
#ifdef DEBUG
    fprintf(stderr, "line %d Start generate if stmt\n", __LINE__);
#endif
    link_node_st *if_link_node = link_list_pop(token_list);
    char *if_link_node_data = link_node_get_data(if_link_node);
    if (strcmp(if_link_node_data, "if") != 0) {
        raise_syntax_error(__LINE__, "expected: if");
    }
    parsing_tree_st *if_tree_node = parsing_tree_new(strdup(if_link_node_data), free);
    link_node_free(if_link_node);

    link_node_st *left_parenthesis = link_list_pop(token_list);
    char *left_parenthesis_data = link_node_get_data(left_parenthesis);
    if (strcmp(left_parenthesis_data, "(") != 0) {
        raise_syntax_error(__LINE__, "expected: (");
    }
    parsing_tree_st *left_parenthesis_tree_node = parsing_tree_new(strdup(left_parenthesis_data), free);
    link_node_free(left_parenthesis);

    parsing_tree_st *boolean_expr = generate_boolean_expre(token_list, symbol_table);

    link_node_st *right_parenthesis = link_list_pop(token_list);
    char *right_parenthesis_data = link_node_get_data(right_parenthesis);
    if (strcmp(right_parenthesis_data, ")") != 0) {
        raise_syntax_error(__LINE__, "expected: )");
    }
    parsing_tree_st *right_parenthesis_tree_node = parsing_tree_new(strdup(right_parenthesis_data), free);
    link_node_free(right_parenthesis);

    link_node_st *then_link_node = link_list_pop(token_list);
    char *then_link_node_data = link_node_get_data(then_link_node);
    if (strcmp(then_link_node_data, "then")) {
        raise_syntax_error(__LINE__, "expected: then");
    }
    parsing_tree_st *then_tree_node = parsing_tree_new(strdup(then_link_node_data), free);
    link_node_free(then_link_node);
    
    link_node_st *left_bracket = link_list_pop(token_list);
    char *left_bracket_data = link_node_get_data(left_bracket);
    if (strcmp(left_bracket_data, "{") != 0) {
        raise_syntax_error(__LINE__, "expected: {");
    }
    parsing_tree_st *left_bracket_tree_node = parsing_tree_new(strdup(left_bracket_data), free);
    link_node_free(left_bracket);
#ifdef DEBUG
    fprintf(stderr, "line: %d Generate the stmt list in brackets\n", __LINE__);
#endif
    parsing_tree_st *stmt_list = generate_stmt_list(token_list, symbol_table);

    link_node_st *right_bracket = link_list_pop(token_list);
    char *right_bracket_data = link_node_get_data(right_bracket);
    if (strcmp(right_bracket_data, "}") != 0) {
        raise_syntax_error(__LINE__, "expected: }");
    }
    parsing_tree_st *right_bracket_tree_node = parsing_tree_new(strdup(right_bracket_data), free);
    link_node_free(right_bracket);

    parsing_tree_set_sibling(if_tree_node, left_parenthesis_tree_node);
    parsing_tree_set_sibling(left_parenthesis_tree_node, boolean_expr);
    parsing_tree_set_sibling(boolean_expr, right_parenthesis_tree_node);
    parsing_tree_set_sibling(right_parenthesis_tree_node, then_tree_node);
    parsing_tree_set_sibling(then_tree_node, left_bracket_tree_node);
    parsing_tree_set_sibling(left_bracket_tree_node, stmt_list);
    parsing_tree_set_sibling(stmt_list, right_bracket_tree_node);

    link_node_st *else_link_node = link_list_top(token_list);
    char *else_link_node_data = link_node_get_data(else_link_node);
    
    if (strcmp(else_link_node_data, "else") == 0) {

        else_link_node = link_list_pop(token_list);
        parsing_tree_st *else_tree_node = parsing_tree_new(strdup(else_link_node_data), free);
        link_node_free(else_link_node);

        link_node_st *left_bracket2 = link_list_pop(token_list);
        char *left_bracket_data2 = link_node_get_data(left_bracket2);
        if (strcmp(left_bracket_data2, "{") != 0 ) {
            raise_syntax_error(__LINE__, "expected {");
        }
        parsing_tree_st *left_bracket_tree_node2 = parsing_tree_new(strdup(left_bracket_data2), free);
        link_node_free(left_bracket2);
        parsing_tree_st *stmt_list2 = generate_stmt_list(token_list, symbol_table);
    
        link_node_st *right_bracket2 = link_list_pop(token_list);
        char *right_bracket_data2 = link_node_get_data(right_bracket2);
        if (strcmp(right_bracket_data2, "}") != 0) {
            raise_syntax_error(__LINE__, "expected }");
        }
        parsing_tree_st *right_bracket_tree_node2 = parsing_tree_new(strdup(right_bracket_data2), free);
        link_node_free(right_bracket2);

        parsing_tree_set_sibling(right_bracket_tree_node, else_tree_node);
        parsing_tree_set_sibling(else_tree_node, left_bracket_tree_node2);
        parsing_tree_set_sibling(left_bracket_tree_node2, stmt_list2);
        parsing_tree_set_sibling(stmt_list2, right_bracket_tree_node2);
    }

    return if_tree_node;
}

/**
 * @brief: generate the for statement accordig to the grammar rule
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated for statement
 */
static parsing_tree_st *generate_for_stmt(link_list_st *token_list, symbol_table_st *symbol_table) {
#ifdef DEBUG
    fprintf(stderr, "line %d Start generate for stmt\n", __LINE__);
#endif
    link_node_st *for_link_node = link_list_pop(token_list);
    char *for_link_node_data = link_node_get_data(for_link_node);
    if (strcmp(for_link_node_data, "for") != 0) {
        raise_syntax_error(__LINE__, "expected: for");
    }
    link_node_free(for_link_node);
    parsing_tree_st *for_tree_node = parsing_tree_new("for", NULL);

    link_node_st *id_link_node = link_list_pop(token_list);
    char *id_link_node_data = link_node_get_data(id_link_node);
    if(symbol_table_lookup(symbol_table, id_link_node_data) != IDENTIFIER) {
        raise_syntax_error(__LINE__, "expected: IDENTIFIER");
    }
    parsing_tree_st *id_tree_node = parsing_tree_new(strdup(id_link_node_data), free);
    link_node_free(id_link_node);

    link_node_st *from_link_node = link_list_pop(token_list);
    char *from_link_node_data = link_node_get_data(from_link_node);
    if (strcmp(from_link_node_data, "from") != 0){
        raise_syntax_error(__LINE__, "expected: from");
    }
    link_node_free(from_link_node);
    parsing_tree_st *from_tree_node = parsing_tree_new("from", NULL);
#ifdef DEBUG
    fprintf(stderr, "line %d The first element of the first expression is %s\n", 
                __LINE__, link_node_get_data(link_list_top(token_list)));
#endif
    parsing_tree_st *expre1 = generate_expre(token_list, symbol_table);
#ifdef DEBUG
    fprintf(stderr, "line %d First expression generated\n", __LINE__);
#endif
    link_node_st *to_link_node = link_list_pop(token_list);
    char *to_link_node_data = link_node_get_data(to_link_node);
    if (strcmp(to_link_node_data, "to") != 0 && strcmp(to_link_node_data, "downto") != 0) {
        raise_syntax_error(__LINE__, "expected: to or downto");
    } 
    
    parsing_tree_st *to_tree_node = parsing_tree_new(strdup(to_link_node_data), free);
    link_node_free(to_link_node);

    parsing_tree_st *expre2 = generate_expre(token_list, symbol_table);
#ifdef DEBUG
    fprintf(stderr, "line %d Second expression generated\n", __LINE__);
#endif
    link_node_st *step_link_node = link_list_pop(token_list);
    char *step_link_node_data = link_node_get_data(step_link_node);
    if (strcmp(step_link_node_data, "step") != 0) {
        raise_syntax_error(__LINE__, "expected: step");
    }
    link_node_free(step_link_node);
    parsing_tree_st *step_tree_node = parsing_tree_new("step", NULL);

    parsing_tree_st *expre3 = generate_expre(token_list, symbol_table);
#ifdef DEBUG
    fprintf(stderr, "line %d Third expression generated\n", __LINE__);
#endif
    link_node_st *left_bracket_link_node = link_list_pop(token_list);
    char *left_bracket_link_node_data = link_node_get_data(left_bracket_link_node);
    if (strcmp(left_bracket_link_node_data, "{") != 0) {
        raise_syntax_error(__LINE__, "expected: {");
    }
    link_node_free(left_bracket_link_node);
    parsing_tree_st *left_bracket_tree_node = parsing_tree_new("{", NULL);

    parsing_tree_st *stmt_list = generate_stmt_list(token_list, symbol_table);

    link_node_st *right_bracket_link_node = link_list_pop(token_list);
    char *right_bracket_link_node_data = link_node_get_data(right_bracket_link_node);
    if (strcmp(right_bracket_link_node_data, "}") != 0) {
        raise_syntax_error(__LINE__, "expected: }");
    } 
    link_node_free(right_bracket_link_node);
    parsing_tree_st *right_bracket_tree_node = parsing_tree_new("}", NULL);

    parsing_tree_set_sibling(for_tree_node, id_tree_node);
    parsing_tree_set_sibling(id_tree_node, from_tree_node);
    parsing_tree_set_sibling(from_tree_node, expre1);
    parsing_tree_set_sibling(expre1, to_tree_node);
    parsing_tree_set_sibling(to_tree_node, expre2);
    parsing_tree_set_sibling(expre2, step_tree_node);
    parsing_tree_set_sibling(step_tree_node, expre3);
    parsing_tree_set_sibling(expre3, left_bracket_tree_node);
    parsing_tree_set_sibling(left_bracket_tree_node, stmt_list);
    parsing_tree_set_sibling(stmt_list, right_bracket_tree_node);
    return for_tree_node;
}

/**
 * @brief: generate the boolean expression accordig to the grammar rule
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated boolean expression
 */
static parsing_tree_st *generate_boolean_expre(link_list_st *token_list, symbol_table_st *symbol_table) {
#ifdef DEBUG
    fprintf(stderr, "line %d Start generate boolean_expr\n", __LINE__);
#endif
    parsing_tree_st *boolean_expr = parsing_tree_new("boolean_expr", NULL);
    link_node_st *first_element = link_list_top(token_list);
    char *first_element_data = link_node_get_data(first_element);
    if (strcmp(first_element_data, "true") == 0 || strcmp(first_element_data, "false") == 0) {
        parsing_tree_st *boolean_value = generate_boolean_value(token_list, symbol_table);
        parsing_tree_set_child(boolean_expr, boolean_value);
    } else {
        parsing_tree_st *left_expre = generate_expre(token_list, symbol_table);
#ifdef DEBUG
        fprintf(stderr, "line: %d left_expre generated\n", __LINE__);
#endif
        link_node_st *operator_link_node = link_list_pop(token_list);
        char *operator_link_node_data = link_node_get_data(operator_link_node);
        if (symbol_table_lookup(symbol_table, operator_link_node_data) != BOOLEAN_OP) {
            raise_syntax_error(__LINE__, "expected: boolean operators");
        }
        parsing_tree_st *operator_tree_node = parsing_tree_new(strdup(operator_link_node_data), free);
        link_node_free(operator_link_node);
        
        parsing_tree_st *right_expre = generate_expre(token_list, symbol_table);
#ifdef DEBUG
        fprintf(stderr, "line: %d right_expre generated\n", __LINE__);
#endif
        parsing_tree_set_child(boolean_expr, left_expre);
        parsing_tree_set_sibling(left_expre, operator_tree_node);
        parsing_tree_set_sibling(operator_tree_node, right_expre);
    }
    return boolean_expr;
}

/**
 * @brief: generate the boolean value accordig to the grammar rule
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated boolean value
 */
static parsing_tree_st *generate_boolean_value(link_list_st *token_list, symbol_table_st *symbol_table) {
#ifdef DEBUG
    fprintf(stderr, "line %d Start generate boolean_value\n", __LINE__);
#endif
    parsing_tree_st *boolean_value = parsing_tree_new("boolean_value", NULL);
    link_node_st *value_link_node = link_list_pop(token_list);
    char *value_link_node_data = link_node_get_data(value_link_node);
    parsing_tree_st *value_tree_node = parsing_tree_new(strdup(value_link_node_data), free);
    link_node_free(value_link_node);
    parsing_tree_set_child(boolean_value, value_tree_node);
    return boolean_value;
}

/**
 * @brief: generate the statement list recursively accordig to the grammar rule
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated parsing tree
 */
static parsing_tree_st *generate_stmt(link_list_st *token_list, symbol_table_st *symbol_table) {
    link_node_st *stmt_type = link_list_top(token_list);
    parsing_tree_st *stmt_root = parsing_tree_new("stmt", NULL);
    parsing_tree_st *stmt_tree = NULL;
    parsing_tree_st *stmt_child = NULL;
    char *node_data = link_node_get_data(stmt_type);

    if (strcmp(node_data, "var") == 0) {
        stmt_tree = parsing_tree_new("decl_stmt", NULL);
        stmt_child = generate_decl_stmt(token_list, symbol_table);
    } else if (strcmp(node_data, "print") == 0) {
        stmt_tree = parsing_tree_new("print_stmt", NULL);
        stmt_child = generate_print_stmt(token_list, symbol_table);
    } else if (strcmp(node_data, "if") == 0) {
        stmt_tree = parsing_tree_new("if_stmt", NULL);
        stmt_child = generate_if_stmt(token_list, symbol_table);
    } else if (strcmp(node_data, "for") == 0) {
        stmt_tree = parsing_tree_new("for_stmt", NULL);
        stmt_child = generate_for_stmt(token_list, symbol_table);
    } else if (symbol_table_lookup(symbol_table, node_data) == IDENTIFIER) {
        stmt_tree = parsing_tree_new("assign_stmt", NULL);
        stmt_child = generate_assign_stmt(token_list, symbol_table);
    } else {
        #ifdef DEBUG
        printf("Can not generate statement\n");
        #endif
        fprintf(stderr, "%s\n", node_data);
        raise_syntax_error(__LINE__, "unsupported statement");
    }

    parsing_tree_set_child(stmt_tree, stmt_child);
    parsing_tree_set_child(stmt_root, stmt_tree);

    return stmt_root;
}

/**
 * @brief: generate the declare statement accordig to the grammar rule
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated root node of the declare statement subtree
 */
static parsing_tree_st *generate_decl_stmt(link_list_st *token_list, symbol_table_st *symbol_table) {
    link_node_st *var_node = link_list_pop(token_list);
    char *node_data = link_node_get_data(var_node);
    if (strcmp(node_data, "var") != 0)
        raise_syntax_error(__LINE__, "expected: var");

    parsing_tree_st *var_tree_node = parsing_tree_new(strdup(node_data), free);
    link_node_free(var_node);

    link_node_st *id_node = link_list_pop(token_list);
    node_data = link_node_get_data(id_node);

    if (symbol_table_lookup(symbol_table, node_data) != IDENTIFIER)
        raise_syntax_error(__LINE__, "expected: IDENTIFIER");

    parsing_tree_st *id_tree_node = parsing_tree_new(strdup(node_data), free);
    parsing_tree_set_sibling(var_tree_node, id_tree_node);
    link_node_free(id_node);

    return var_tree_node;
}

/**
 * @brief: generate the factor accordig to the grammar rule
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated first term of the expression
 */
static parsing_tree_st *generate_factor(link_list_st *token_list, symbol_table_st *symbol_table) {
    link_node_st *first_element = link_list_pop(token_list);
    char* first_element_data = link_node_get_data(first_element);
    int type_index = symbol_table_lookup(symbol_table, first_element_data);
    parsing_tree_st *factor = parsing_tree_new("factor", NULL);

    if (type_index == OPEN_PARENTHESES) {
        parsing_tree_st *left_parenthesis = parsing_tree_new(strdup(first_element_data), free);
        link_node_free(first_element);
        parsing_tree_st *expr_tree_node = generate_expre(token_list, symbol_table);
        link_node_st *right_parenthesis_link_node = link_list_pop(token_list);
        char* right_parenthesis_link_node_data = link_node_get_data(right_parenthesis_link_node);
        if (strcmp(right_parenthesis_link_node_data, ")") != 0) {
            raise_syntax_error(__LINE__, "expected: )");
        }
        parsing_tree_st *right_parnthesis = parsing_tree_new(strdup(right_parenthesis_link_node_data), free);
        link_node_free(right_parenthesis_link_node);
        parsing_tree_set_child(factor, left_parenthesis);
        parsing_tree_set_sibling(left_parenthesis, expr_tree_node);
        parsing_tree_set_sibling(expr_tree_node, right_parnthesis);
    } else if (type_index == IDENTIFIER || type_index == NUMBER) {
        parsing_tree_st *number_or_id = parsing_tree_new(strdup(first_element_data), free);
        parsing_tree_set_child(factor, number_or_id);
        link_node_free(first_element);     
    } else {
        #ifdef DEBUG
        printf("What we got is %s\n", first_element_data);
        #endif
        raise_syntax_error(__LINE__, "expected: ( or IDENTIFIER or NUMBER");
    }
    return factor;
}

/**
 * @brief: generate the res1 accordig to the grammar rule
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated res1 of the expression
 */
static parsing_tree_st *generate_res1(link_list_st *token_list, symbol_table_st *symbol_table) {
    parsing_tree_st *res1 = parsing_tree_new("res1", NULL);
    link_node_st *operator = link_list_top(token_list);
    char *operator_data = link_node_get_data(operator);
    if (strcmp(operator_data, "+") == 0 || 
        strcmp(operator_data, "-") == 0){

        operator = link_list_pop(token_list);
        parsing_tree_st *operator_tree_node = parsing_tree_new(strdup(operator_data), free);
        link_node_free(operator);
        parsing_tree_st *term = generate_term(token_list, symbol_table);
        parsing_tree_st *res1_second = generate_res1(token_list, symbol_table);
        parsing_tree_set_child(res1, operator_tree_node);
        parsing_tree_set_sibling(operator_tree_node, term);
        parsing_tree_set_sibling(term, res1_second);
    }
    return res1;
}

/**
 * @brief: generate the res2 accordig to the grammar rule
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated res2 of the expression
 */
static parsing_tree_st *generate_res2(link_list_st *token_list, symbol_table_st *symbol_table) {
    link_node_st *operator = link_list_top(token_list);
    char *operator_data = link_node_get_data(operator);
    parsing_tree_st *res2 = parsing_tree_new("res2", NULL);
    if (strcmp(operator_data, "*") == 0 || 
        strcmp(operator_data, "/") == 0 || 
        strcmp(operator_data, "%") == 0) {

        operator = link_list_pop(token_list);
        parsing_tree_st *operator_tree_node = parsing_tree_new(strdup(operator_data), free);
        link_node_free(operator);
        parsing_tree_st *factor = generate_factor(token_list, symbol_table);
        parsing_tree_st *res2_second = generate_res2(token_list, symbol_table);
        parsing_tree_set_child(res2, operator_tree_node);
        parsing_tree_set_sibling(operator_tree_node, factor);
        parsing_tree_set_sibling(factor, res2_second);

    }
    return res2;
}

/**
 * @brief: generate the term accordig to the grammar rule
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated first term of the expression
 */
static parsing_tree_st *generate_term(link_list_st *token_list, symbol_table_st *symbol_table) {
    parsing_tree_st *term = parsing_tree_new("term", NULL);
    parsing_tree_st *factor = generate_factor(token_list, symbol_table);
    parsing_tree_st *res2 = generate_res2(token_list, symbol_table);
    parsing_tree_set_child(term, factor);
    parsing_tree_set_sibling(factor, res2);
    return term;
}

/**
 * @brief: generate the expression accordig to the grammar rule
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated first term of the expression
 */
static parsing_tree_st *generate_expre(link_list_st *token_list, symbol_table_st *symbol_table) {
    parsing_tree_st *term = generate_term(token_list, symbol_table);
    parsing_tree_st *res1 = generate_res1(token_list, symbol_table);
    parsing_tree_st *expre = parsing_tree_new("expr", NULL);
    parsing_tree_set_child(expre, term);
    parsing_tree_set_sibling(term, res1);
    return expre;
}

/**
 * @brief: generate the print statement accordig to the grammar rule
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated root node of the print statement subtree
 */
static parsing_tree_st *generate_print_stmt(link_list_st *token_list, symbol_table_st *symbol_table) {
    link_node_st *print_node = link_list_pop(token_list);
    char *node_data = link_node_get_data(print_node);
    if (strcmp(node_data, "print") != 0)
        raise_syntax_error(__LINE__, "expected: print");
    parsing_tree_st *print_tree_node = parsing_tree_new(strdup(node_data), free);
    link_node_free(print_node);
    parsing_tree_st *expr_tree_node = generate_expre(token_list, symbol_table);
    parsing_tree_set_sibling(print_tree_node, expr_tree_node);

    return print_tree_node;
}

/**
 * @brief: generate the assignment statement accordig to the grammar rule
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated root node of the assignment statement subtree
 */
static parsing_tree_st *generate_assign_stmt(link_list_st *token_list, symbol_table_st *symbol_table) {
    link_node_st *assign_node = link_list_pop(token_list);
    char *node_data = link_node_get_data(assign_node);
    if (symbol_table_lookup(symbol_table, node_data) != IDENTIFIER)
        raise_syntax_error(__LINE__, "expected: IDENTIFIER");

    parsing_tree_st *id_tree_node = parsing_tree_new(strdup(node_data), free);
    link_node_free(assign_node);
    link_node_st *is_node = link_list_pop(token_list);
    node_data = link_node_get_data(is_node);
    if (strcmp(node_data, "is") != 0)
        raise_syntax_error(__LINE__, "expected: is");

    parsing_tree_st *is_tree_node = parsing_tree_new(strdup(node_data), free);
    link_node_free(is_node);
    parsing_tree_set_sibling(id_tree_node, is_tree_node);
    parsing_tree_st *expr_tree_node = generate_expre(token_list, symbol_table);
    parsing_tree_set_sibling(is_tree_node, expr_tree_node);
    return id_tree_node;
}

/**
 * @brief: get the token_list and symbol_table parse it into a parsing tree 
 * @param: pointers to token list 
 * @param: pointers to symbol table
 * @return: the pointer to generated parsing tree
 */
parsing_tree_st *syntax_analysis(link_list_st *token_list, symbol_table_st *symbol_table) {
    if (token_list == NULL || symbol_table == NULL) {
        return NULL;
    }
    /* create 'program' node and 'sttm_list' node */
    /* set the 'stmt_list' node as child node of 'program' node */
    parsing_tree_st *program_tree_node = parsing_tree_new("program", NULL);
    parsing_tree_st *stmt_list_tree_node = generate_stmt_list(token_list, symbol_table);
    parsing_tree_set_child(program_tree_node, stmt_list_tree_node);
    
    return program_tree_node;
}

static parsing_tree_st *generate_stmt_list(link_list_st *token_list, symbol_table_st *symbol_table) {
    parsing_tree_st *root = NULL;
    parsing_tree_st *stmt_list_tree_node = parsing_tree_new("stmt_list", NULL);
    root = stmt_list_tree_node;
    while (1) {
        parsing_tree_st *stmt = generate_stmt(token_list, symbol_table);
        parsing_tree_set_child(stmt_list_tree_node, stmt);

        link_node_st *semicolon_node = link_list_pop(token_list);
        char *node_data = link_node_get_data(semicolon_node);
        if (symbol_table_lookup(symbol_table, node_data) != DELIMITER)
            raise_syntax_error(__LINE__, "expected: DELIMITER");
        link_node_free(semicolon_node);
        parsing_tree_st *semicolon = parsing_tree_new(";", NULL);
        parsing_tree_set_sibling(stmt, semicolon);

        if (link_list_top(token_list) == NULL || 
                        symbol_table_lookup(symbol_table,
                                link_node_get_data(
                                    link_list_top(token_list))) == CLOSE_CURLY_BRACKETS)
            break;
        stmt_list_tree_node = parsing_tree_new("stmt_list", NULL);
        parsing_tree_set_sibling(semicolon, stmt_list_tree_node);
    }
    return root;    
}

#ifdef XTEST

/**
 * @brief: print the data of one node in the parsing tree
 * @param: node of parsing tree
 * @return: no return value
 */
int print_data(parsing_tree_st *parsing_tree_node, void *cb_data){
    if (parsing_tree_node != NULL)
        printf("%s\n", (char *)parsing_tree_get_data(parsing_tree_node));
    return TREE_TRAVERSE_CONTINUE;
}

int print_list_data(link_node_st *node, void *cb_data) {
    if (node != NULL)
        printf("%s\n", (char *)link_node_get_data(node));
    return LINK_LIST_CONTINUE;
}

void test_case_two() {
    printf("----------------------------------------------\n");
    printf("Begin of test case of two\n");
    /* set up phase*/
    /* create a mockup token list */
    link_list_st *token_list = link_list_init();
    link_node_st *var_node = link_node_new("var", NULL);
    link_node_st *id_node = link_node_new("i", NULL);
    link_node_st *semicolon = link_node_new(";", NULL);
    link_list_append(token_list, var_node);
    link_list_append(token_list, id_node);
    link_list_append(token_list, semicolon);

    var_node = link_node_new("var", NULL);
    id_node = link_node_new("j", NULL);
    semicolon = link_node_new(";", NULL);
    link_list_append(token_list, var_node);
    link_list_append(token_list, id_node);
    link_list_append(token_list, semicolon);
    /* create a mockup symbol table */
    symbol_table_st *symbol_table = symbol_table_init();
    symbol_table_insert(symbol_table, "i", IDENTIFIER);
    symbol_table_insert(symbol_table, "j", IDENTIFIER);
    link_list_traverse(token_list, print_list_data, NULL);
    /* generate the parsing tree */
    parsing_tree_st *decl_stmt = syntax_analysis(token_list, symbol_table);
    parsing_tree_prefix_traverse(decl_stmt, print_data, NULL);
}


/**
 * @brief: this test case generate a tree from a declare statement and print it out
 * @param: (no parameter)
 * @return: the pointer to generated root node of the assignment statement subtree
 */
void test_case_one() {
    printf("----------------------------------------------\n");
    printf("Begin of test case of one\n");
    /* set up phase*/
    /* create a mockup token list */
    link_list_st *token_list = link_list_init();
    link_node_st *var_node = link_node_new("var", NULL);
    link_node_st *id_node = link_node_new("i", NULL);
    link_node_st *semicolon = link_node_new(";", NULL);
    link_list_append(token_list, var_node);
    link_list_append(token_list, id_node);
    link_list_append(token_list, semicolon);
    /* create a mockup symbol table */
    symbol_table_st *symbol_table = symbol_table_init();
    symbol_table_insert(symbol_table, "i", IDENTIFIER);
    link_list_traverse(token_list, print_list_data, NULL);
    /* generate the parsing tree */
    parsing_tree_st *decl_stmt = syntax_analysis(token_list, symbol_table);
    parsing_tree_prefix_traverse(decl_stmt, print_data, NULL);
}

/**
 * @brief: this test case generate a tree from a print statement that contains expression and print it out
 * @param: (no parameter)
 * @return: the pointer to generated root node of the assignment statement subtree
 */
void test_case_three() {
    printf("----------------------------------------------\n");
    printf("Begin of test case of three\n");
    /* set up phase*/
    /* create a mockup token list */
    link_list_st *token_list = link_list_init();
    link_node_st *print_node = link_node_new("print", NULL);
    link_node_st *id_node = link_node_new("i", NULL);
    link_node_st *plus_node = link_node_new("+", NULL);
    link_node_st *one_node = link_node_new("i", NULL);
    // link_node_st *plus_node2 = link_node_new("+", NULL);
    // link_node_st *one_node2 = link_node_new("1", NULL);
    link_node_st *semicolon = link_node_new(";", NULL);
    link_list_append(token_list, print_node);
    link_list_append(token_list, id_node);
    link_list_append(token_list, plus_node);
    link_list_append(token_list, one_node);
    // link_list_append(token_list, plus_node2);
    // link_list_append(token_list, one_node2);
    link_list_append(token_list, semicolon);


    /* create a mockup symbol table */
    symbol_table_st *symbol_table = symbol_table_init();
    symbol_table_insert(symbol_table, "i", IDENTIFIER);
    link_list_traverse(token_list, print_list_data, NULL);
    /* generate the parsing tree */
    parsing_tree_st *decl_stmt = syntax_analysis(token_list, symbol_table);
    parsing_tree_prefix_traverse(decl_stmt, print_data, NULL);
}

/**
 * @brief: this test case generate a tree from a if statement that contains declare statement
 * @param: (no parameter)
 * @return: the pointer to generated root node of the assignment statement subtree
 */
void test_case_four() {
    printf("----------------------------------------------\n");
    printf("Begin of test case of four\n");
    /* set up phase*/
    /* create a mockup token list */
    link_list_st *token_list = link_list_init();
    link_node_st *if_node = link_node_new("if", NULL);
    link_node_st *left_paranthesis_node = link_node_new("(", NULL);
    link_node_st *i_node = link_node_new("i", NULL);
    link_node_st *equal_node = link_node_new("=", NULL);
    link_node_st *j_node = link_node_new("j", NULL);
    link_node_st *right_paranthesis_node = link_node_new(")", NULL);

    link_node_st *then_node = link_node_new("then", NULL);
    link_node_st *left_bracket= link_node_new("{", NULL);
    link_node_st *var_node = link_node_new("var", NULL);
    link_node_st *i_node2 = link_node_new("i", NULL);
    link_node_st *semicolon = link_node_new(";", NULL);
    link_node_st *right_bracket= link_node_new("}", NULL);

    link_node_st *else_node = link_node_new("else", NULL);
    link_node_st *left_bracket2 = link_node_new("{", NULL);
    link_node_st *var_node2 = link_node_new("var", NULL);
    link_node_st *i_node3 = link_node_new("i", NULL);
    link_node_st *semicolon2 = link_node_new(";", NULL);
    link_node_st *right_bracket2 = link_node_new("}", NULL);
    link_node_st *semicolon3 = link_node_new(";", NULL);

    link_list_append(token_list, if_node);
    link_list_append(token_list, left_paranthesis_node);
    link_list_append(token_list, i_node);
    link_list_append(token_list, equal_node);
    link_list_append(token_list, j_node);
    link_list_append(token_list, right_paranthesis_node);
    link_list_append(token_list, then_node);
    link_list_append(token_list, left_bracket);
    link_list_append(token_list, var_node);
    link_list_append(token_list, i_node2);
    link_list_append(token_list, semicolon);
    link_list_append(token_list, right_bracket);
    link_list_append(token_list, else_node);
    link_list_append(token_list, left_bracket2);
    link_list_append(token_list, var_node2);
    link_list_append(token_list, i_node3);
    link_list_append(token_list, semicolon2);
    link_list_append(token_list, right_bracket2);
    link_list_append(token_list, semicolon3);

    /* create a mockup symbol table */
    symbol_table_st *symbol_table = symbol_table_init();
    symbol_table_insert(symbol_table, "i", IDENTIFIER);
    symbol_table_insert(symbol_table, "j", IDENTIFIER);
    link_list_traverse(token_list, print_list_data, NULL);
    /* generate the parsing tree */
    parsing_tree_st *decl_stmt = syntax_analysis(token_list, symbol_table);
    parsing_tree_prefix_traverse(decl_stmt, print_data, NULL);

}

void test_case_five() {
    printf("----------------------------------------------\n");
    printf("Begin of test case of four\n");
    link_list_st *token_list = link_list_init();
    /* set up phase*/
    /* create a mockup token list */
    link_node_st *for_node = link_node_new("for", NULL);
    link_node_st *i_node1 = link_node_new("i", NULL);
    link_node_st *from_node = link_node_new("from", NULL);
    link_node_st *i_node2 = link_node_new("i", NULL);
    link_node_st *to_node = link_node_new("to", NULL);
    link_node_st *step_node = link_node_new("step", NULL);
    link_node_st *mul_node = link_node_new("*", NULL);
    link_node_st *left_bracket_node = link_node_new("{", NULL);
    link_node_st *var_node = link_node_new("var", NULL);
    link_node_st *i_node3 = link_node_new("i", NULL);
    link_node_st *semicolon_node = link_node_new(";", NULL);
    link_node_st *right_bracket_node = link_node_new("}", NULL);
    link_node_st *semicolon_node2 = link_node_new(";", NULL);


    link_node_st *i_node_extra = link_node_new("i", NULL);
    link_node_st *i_node_extra2 = link_node_new("i", NULL);
    link_node_st *i_node_extra333 = link_node_new("i", NULL);


    link_list_append(token_list, for_node);
    link_list_append(token_list, i_node1);
    link_list_append(token_list, from_node);
    link_list_append(token_list, i_node2);
    

    link_list_append(token_list, to_node);
    // link_list_append(token_list, one_node2);
    link_list_append(token_list, i_node_extra333);
    link_list_append(token_list, step_node);
    // link_list_append(token_list, one_node3);
    link_list_append(token_list, i_node_extra);
    link_list_append(token_list, mul_node);
    link_list_append(token_list, i_node_extra2);

    // link_list_append(token_list, one_node4);
    link_list_append(token_list, left_bracket_node);
    link_list_append(token_list, var_node);
    link_list_append(token_list, i_node3);
    link_list_append(token_list, semicolon_node);
    link_list_append(token_list, right_bracket_node);
    link_list_append(token_list, semicolon_node2);
    
    /* create a mockup symbol table */
    symbol_table_st *symbol_table = symbol_table_init();
    symbol_table_insert(symbol_table, "i", IDENTIFIER);
    link_list_traverse(token_list, print_list_data, NULL);

    
    /* generate the parsing tree */
    parsing_tree_st *program = syntax_analysis(token_list, symbol_table);
    parsing_tree_prefix_traverse(program, print_data, NULL);
}

int main()
{
    test_case_one();
    test_case_two();
    test_case_three();
    test_case_four();
    test_case_five();

    return 0;
}

#endif
